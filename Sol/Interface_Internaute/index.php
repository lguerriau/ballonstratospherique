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

// --- 1. RÉCUPÉRATION DES DONNÉES TÉLÉMÉTRIE ---
try {
    $requete = $db->query('SELECT temp, humidity, pressure, time FROM TELEMETRIE ORDER BY time DESC LIMIT 50');
    $toutes_donnees = $requete->fetchAll(PDO::FETCH_ASSOC);
    
    $donnees = !empty($toutes_donnees) ? $toutes_donnees[0] : false;
} catch (Exception $e) {
    $donnees = false;
    $toutes_donnees = [];
    $erreur_sql = $e->getMessage();
}

// --- 2. RÉCUPÉRATION DES IMAGES SSTV ---
try {
    $requete_images = $db->query('SELECT id_image, chemin_image, horodatage_image FROM IMAGE ORDER BY id_image DESC LIMIT 24');
    $images_sstv = $requete_images->fetchAll(PDO::FETCH_ASSOC);
} catch (Exception $e) {
    $images_sstv = [];
    $erreur_sstv = $e->getMessage();
}

// Gestion des temps
$date_affichage = "Aucune donnée";
$prochaine_affichage = "Aucune donnée";
if ($donnees && isset($donnees['time'])) {
    if (is_numeric($donnees['time'])) {
        $timestamp_ajuste = $donnees['time'] + 7200;
        $date_affichage = date('d/m/Y H:i:s', $timestamp_ajuste);
        $prochaine_affichage = date('d/m/Y H:i:s', $timestamp_ajuste + 600);
    } else {
        $date_affichage = $donnees['time'];
        $prochaine_affichage = "Inconnue";
    }
}

$js_labels = [];
$js_temp = [];
$js_humidity = [];
$js_pressure = [];

$donnees_chronologiques = array_reverse($toutes_donnees);
foreach ($donnees_chronologiques as $row) {
    if (is_numeric($row['time'])) {
        $js_labels[] = date('H:i:s', $row['time'] + 7200);
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
        <title>APRS Mission Control - Télémesure et SSTV</title>
        <link rel="stylesheet" href="style.css">
        <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    </head>
    <body>
        <h1>Télémesure du Ballon Stratosphérique</h1>

        <div class="tabs-container">
            <button class="tab-btn active" onclick="switchTab('telemetrie')">Télémétrie</button>
            <button class="tab-btn" onclick="switchTab('sstv')">Galerie SSTV</button>
        </div>

        <div id="tab-telemetrie" class="tab-content active">
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
                        <div id="history-list" class="history-list"></div>
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
        </div>

        <div id="tab-sstv" class="tab-content">
            <p class="timestamp">Images reçues en direct de l'émetteur embarqué</p>
            
            <div class="sstv-actions">
                <a href="generer_video.php" class="btn-video">Générer la Vidéo Timelapse</a>
            </div>

            <?php if (!empty($images_sstv)): ?>
                <div class="galerie-sstv">
                    <?php foreach ($images_sstv as $img): ?>
                        <div class="photo-card">
                            <img src="ARCHIVE_PHOTOS/<?php echo htmlspecialchars(basename($img['chemin_image'])); ?>" alt="SSTV du ballon">
                            <div class="photo-info">
                                <span class="photo-id">ID: #<?php echo htmlspecialchars($img['id_image']); ?></span>
                                <span class="photo-date">Reçue le : <?php echo htmlspecialchars($img['horodatage_image']); ?></span>
                            </div>
                        </div>
                    <?php endforeach; ?>
                </div>
            <?php else: ?>
                <div class="error-card">
                    <p>Aucune image SSTV enregistrée pour le moment.</p>
                    <?php if (isset($erreur_sstv)): ?>
                        <small>Erreur SQL : <?php echo htmlspecialchars($erreur_sstv); ?></small>
                    <?php endif; ?>
                </div>
            <?php endif; ?>
        </div>

        <script src="js.js"></script>
        <script>
            const labels = <?php echo json_encode($js_labels); ?>;
            const tempData = <?php echo json_encode($js_temp); ?>;
            const humidityData = <?php echo json_encode($js_humidity); ?>;
            const pressureData = <?php echo json_encode($js_pressure); ?>;
            const rawData = <?php echo json_encode($toutes_donnees); ?>;

            const configs = {
                temp: { label: 'Température (°C)', data: tempData, color: '#ff4d4d', unit: '°C' },
                humidity: { label: 'Humidité (%)', data: humidityData, color: '#3498db', unit: '%' },
                pressure: { label: 'Pression (hPa)', data: pressureData, color: '#2ecc71', unit: 'hPa' }
            };

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
                        backgroundColor: configs[currentMetric].color + '1a',
                        borderWidth: 3,
                        tension: 0.2,
                        pointRadius: 2
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    animation: false,
                    plugins: { legend: { labels: { color: '#fff' } } },
                    scales: {
                        x: { grid: { color: '#444' }, ticks: { color: '#bbb' } },
                        y: { grid: { color: '#444' }, ticks: { color: '#bbb' } }
                    }
                }
            });

            function selectMetric(metric) {
                currentMetric = metric;
                localStorage.setItem('selectedMetric', metric);
                
                document.querySelectorAll('.dashboard .data-card').forEach(card => card.classList.remove('active'));
                document.getElementById('btn-' + metric).classList.add('active');

                historyChart.data.datasets[0].label = configs[metric].label;
                historyChart.data.datasets[0].data = configs[metric].data;
                historyChart.data.datasets[0].borderColor = configs[metric].color;
                historyChart.data.datasets[0].backgroundColor = configs[metric].color + '1a';
                historyChart.update();

                renderHistoryList(metric);
            }

            function renderHistoryList(metric) {
                const listContainer = document.getElementById('history-list');
                const titleContainer = document.getElementById('history-title');
                if(!listContainer || !titleContainer) return;
                
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

            if(rawData.length > 0) {
                selectMetric(currentMetric);
            }
        </script>
    </body>
</html>