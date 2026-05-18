<?php
// Connexion à la base de données
$host = '172.18.58.85';
$dbname = 'ballon2026';
$user = 'root';
$pass = 'toto';

try {
    $db = new PDO("mysql:host=$host;dbname=$dbname", $user, $pass);
    $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
} catch (Exception $e) {
    die('Erreur de connexion : ' . $e->getMessage());
}

// Récupération des 50 dernières lignes pour alimenter le graphique et l'historique
try {
    $requete = $db->query('SELECT temp, humidity, pressure, time FROM TELEMETRIE ORDER BY time DESC LIMIT 50');
    $toutes_donnees = $requete->fetchAll(PDO::FETCH_ASSOC);
    
    // La première ligne correspond à la donnée la plus récente
    $donnees = !empty($toutes_donnees) ? $toutes_donnees[0] : false;
} catch (Exception $e) {
    $donnees = false;
    $toutes_donnees = [];
    $erreur_sql = $e->getMessage();
}

// Gestion du décalage horaire (+2 heures) et de la prochaine mise à jour (+10 min) pour le bandeau principal
$date_affichage = "Aucune donnée";
$prochaine_affichage = "Aucune donnée";
if ($donnees && isset($donnees['time'])) {
    if (is_numeric($donnees['time'])) {
        $timestamp_ajuste = $donnees['time'] + 7200; // +2h
        $date_affichage = date('d/m/Y H:i:s', $timestamp_ajuste);
        
        // Prochaine MAJ = Dernière MAJ + 10 minutes (600 secondes)
        $prochaine_affichage = date('d/m/Y H:i:s', $timestamp_ajuste + 600);
    } else {
        $date_affichage = $donnees['time'];
        $prochaine_affichage = "Inconnue";
    }
}

// Préparation des données chronologiques (du plus ancien au plus récent) pour le graphique Chart.js
$js_labels = [];
$js_temp = [];
$js_humidity = [];
$js_pressure = [];

$donnees_chronologiques = array_reverse($toutes_donnees);
foreach ($donnees_chronologiques as $row) {
    if (is_numeric($row['time'])) {
        $js_labels[] = date('H:i:s', $row['time'] + 7200); // +2 heures également ici
    } else {
        $js_labels[] = $row['time'];
    }
    $js_temp[] = $row['temp'];
    $js_humidity[] = $row['humidity'];
    $js_pressure[] = $row['pressure'];
}
?>

<!DOCTYPE html>
<html lang="fr">
    <head>
        <meta charset="UTF-8">
        <meta http-equiv="refresh" content="600">
        <title>APRS Mission Control - Télémesure</title>
        <link rel="stylesheet" href="style.css">
        <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    </head>
    <body>
        <h1>Télémesure du Ballon Stratosphérique</h1>

        <?php if ($donnees): ?>
            <p class="timestamp">Dernière mise à jour : <?php echo htmlspecialchars($date_affichage); ?></p>
            <p class="timestamp" style="margin-top: -20px; margin-bottom: 30px; color: #888; font-size: 0.95rem;">Prochaine mise à jour : <?php echo htmlspecialchars($prochaine_affichage); ?></p>

            <div class="dashboard">
                <div id="btn-temp" class="data-card card-temp active" onclick="selectMetric('temp')">
                    <h3>Température</h3>
                    <p class="valeur"><?php echo htmlspecialchars($donnees['temp']); ?> °C</p>
                </div>

                <div id="btn-humidity" class="data-card card-humidity" onclick="selectMetric('humidity')">
                    <h3>Humidité</h3>
                    <p class="valeur"><?php echo htmlspecialchars($donnees['humidity']); ?> %</p>
                </div>

                <div id="btn-pressure" class="data-card card-pressure" onclick="selectMetric('pressure')">
                    <h3>Pression</h3>
                    <p class="valeur"><?php echo htmlspecialchars($donnees['pressure']); ?> hPa</p>
                </div>
            </div>

            <div class="analytics-section">
                <div class="chart-container">
                    <canvas id="historyChart"></canvas>
                </div>
                <div class="history-container">
                    <h3 id="history-title">Historique</h3>
                    <div id="history-list" class="history-list">
                        </div>
                </div>
            </div>

        <?php else: ?>
            <div class="error-card">
                <p>Aucune donnée trouvée dans la table TELEMETRIE.</p>
                <?php if (isset($erreur_sql)): ?>
                    <small>Erreur SQL : <?php echo htmlspecialchars($erreur_sql); ?></small>
                <?php endif; ?>
            </div>
        <?php endif; ?>

        <script>
            // Injection des données PHP vers le JavaScript
            const labels = <?php echo json_encode($js_labels); ?>;
            const tempData = <?php echo json_encode($js_temp); ?>;
            const humidityData = <?php echo json_encode($js_humidity); ?>;
            const pressureData = <?php echo json_encode($js_pressure); ?>;
            
            // Liste brute inversée (donnée récente en haut) pour le volet de droite
            const rawData = <?php echo json_encode($toutes_donnees); ?>;

            // Configuration des styles par métrique
            const configs = {
                temp: {
                    label: 'Température (°C)',
                    data: tempData,
                    color: '#ff4d4d',
                    unit: '°C'
                },
                humidity: {
                    label: 'Humidité (%)',
                    data: humidityData,
                    color: '#3498db',
                    unit: '%'
                },
                pressure: {
                    label: 'Pression (hPa)',
                    data: pressureData,
                    color: '#2ecc71',
                    unit: 'hPa'
                }
            };

            // Initialisation du graphique Chart.js (par défaut sur 'temp')
            const ctx = document.getElementById('historyChart').getContext('2d');
            let currentMetric = localStorage.getItem('selectedMetric') || 'temp';
            
            const historyChart = new Chart(ctx, {
                type: 'line',
                data: {
                    labels: labels,
                    datasets: [{
                        label: configs[currentMetric].label,
                        data: configs[currentMetric].data,
                        borderColor: configs[currentMetric].color,
                        backgroundColor: configs[currentMetric].color + '1a', // Couleur transparente pour le fond
                        borderWidth: 3,
                        tension: 0.2,
                        pointRadius: 2
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    animation: false, // <-- AJOUTE CETTE LIGNE ICI
                    plugins: {
                        legend: { labels: { color: '#fff' } }
                    },
                    scales: {
                        x: { grid: { color: '#444' }, ticks: { color: '#bbb' } },
                        y: { grid: { color: '#444' }, ticks: { color: '#bbb' } }
                    }
                }
            });

            // Fonction permettant de changer de métrique lors du clic sur une boîte
            function selectMetric(metric) {
                currentMetric = metric;
                localStorage.setItem('selectedMetric', metric); // Conserve le choix au rafraîchissement
                
                // Mise à jour visuelle des boutons (classes actives)
                document.querySelectorAll('.data-card').forEach(card => card.classList.remove('active'));
                document.getElementById('btn-' + metric).classList.add('active');

                // Mise à jour du graphique
                historyChart.data.datasets[0].label = configs[metric].label;
                historyChart.data.datasets[0].data = configs[metric].data;
                historyChart.data.datasets[0].borderColor = configs[metric].color;
                historyChart.data.datasets[0].backgroundColor = configs[metric].color + '1a';
                historyChart.update();

                // Mise à jour de la liste textuelle historique à droite
                renderHistoryList(metric);
            }

            // Génération de la liste textuelle à droite
            function renderHistoryList(metric) {
                const listContainer = document.getElementById('history-list');
                const titleContainer = document.getElementById('history-title');
                
                titleContainer.textContent = "Log " + metric.charAt(0).toUpperCase() + metric.slice(1);
                listContainer.innerHTML = '';

                rawData.forEach(row => {
                    let timestamp = parseInt(row['time']);
                    let timeStr = isNaN(timestamp) ? row['time'] : new Date((timestamp + 7200) * 1000).toISOString().substr(11, 8);
                    
                    const item = document.createElement('div');
                    item.className = 'history-item';
                    item.innerHTML = `<span class="time">${timeStr}</span> : <span class="val" style="color:${configs[metric].color}">${row[metric]} ${configs[metric].unit}</span>`;
                    listContainer.appendChild(item);
                });
            }

            // Lancement initial basé sur la dernière métrique choisie
            selectMetric(currentMetric);
        </script>
    </body>
</html>