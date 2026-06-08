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
    $requete = $db->query('SELECT temp, humidity, pressure, time FROM TELEMETRIES ORDER BY time DESC LIMIT 50');
    $toutes_donnees = $requete->fetchAll(PDO::FETCH_ASSOC);
    
    $donnees = !empty($toutes_donnees) ? $toutes_donnees[0] : false;
} catch (Exception $e) {
    $donnees = false;
    $toutes_donnees = [];
    $erreur_sql = $e->getMessage();
}

// --- 2. RÉCUPÉRATION DES IMAGES SSTV ---
try {
    $requete_images = $db->query('SELECT id_image, chemin_image, horodatage_image FROM IMAGES ORDER BY id_image DESC LIMIT 24');
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
                    <div class="card-group-tp">
                        <div id="btn-temp" class="data-card card-temp active" onclick="toggleMetric('temp')">
                            <h3>Température</h3>
                            <p class="valeur">
                                <?php
                                $tempC = floatval($donnees['temp']);
                                $tempK = round($tempC + 273.15, 2);
                                echo htmlspecialchars($tempK);
                                ?> °K
                            </p>
                            <p class="valeur-sub">(<?php echo htmlspecialchars($donnees['temp']); ?> °C)</p>
                        </div>
                        <div id="btn-pressure" class="data-card card-pressure" onclick="toggleMetric('pressure')">
                            <h3>Pression</h3>
                            <p class="valeur"><?php echo htmlspecialchars($donnees['pressure']); ?> hPa</p>
                        </div>
                    </div>

                    <div id="btn-humidity" class="data-card card-humidity" onclick="selectMetric('humidity')">
                        <h3>Humidité</h3>
                        <p class="valeur"><?php echo htmlspecialchars($donnees['humidity']); ?> %</p>
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
            const tempData = <?php echo json_encode($js_temp); ?>.map(v => Math.round((parseFloat(v) + 273.15) * 100) / 100);
            const tempDataC = <?php echo json_encode($js_temp); ?>;
            const humidityData = <?php echo json_encode($js_humidity); ?>;
            const pressureData = <?php echo json_encode($js_pressure); ?>;
            const rawData = <?php echo json_encode($toutes_donnees); ?>;

            const configs = {
                temp:     { label: 'Température (°K)', data: tempData,     color: '#ff4d4d', unit: '°K' },
                humidity: { label: 'Humidité (%)',      data: humidityData, color: '#3498db', unit: '%'  },
                pressure: { label: 'Pression (hPa)',    data: pressureData, color: '#2ecc71', unit: 'hPa'}
            };

            // Active metrics state (temp active by default)
            let activeMetrics = new Set(JSON.parse(localStorage.getItem('activeMetrics') || '["temp"]'));

            const ctx = document.getElementById('historyChart').getContext('2d');

            function buildDatasets() {
                return [...activeMetrics].map(metric => ({
                    label: configs[metric].label,
                    data: configs[metric].data,
                    borderColor: configs[metric].color,
                    backgroundColor: configs[metric].color + '1a',
                    borderWidth: 3,
                    tension: 0.2,
                    pointRadius: 2,
                    yAxisID: metric === 'pressure' && activeMetrics.has('temp') ? 'y2' : 'y'
                }));
            }

            function buildScales() {
                const scales = {
                    x: { grid: { color: '#444' }, ticks: { color: '#bbb' } },
                    y: { grid: { color: '#444' }, ticks: { color: '#bbb' }, position: 'left' }
                };
                // If both temp and pressure are active, use dual Y axis
                if (activeMetrics.has('temp') && activeMetrics.has('pressure')) {
                    scales.y2 = {
                        grid: { color: '#333', drawOnChartArea: false },
                        ticks: { color: '#2ecc71' },
                        position: 'right'
                    };
                }
                return scales;
            }

            const historyChart = new Chart(ctx, {
                type: 'line',
                data: { labels: labels, datasets: buildDatasets() },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    animation: false,
                    plugins: { legend: { labels: { color: '#fff' } } },
                    scales: buildScales()
                }
            });

            function refreshChart() {
                historyChart.data.datasets = buildDatasets();
                historyChart.options.scales = buildScales();
                historyChart.update();
                renderHistoryList();
                saveState();
            }

            function saveState() {
                localStorage.setItem('activeMetrics', JSON.stringify([...activeMetrics]));
            }

            function updateCardStates() {
                document.querySelectorAll('.dashboard .data-card').forEach(card => card.classList.remove('active'));
                activeMetrics.forEach(m => {
                    const el = document.getElementById('btn-' + m);
                    if (el) el.classList.add('active');
                });
            }

            // Toggle temp or pressure (they can coexist)
            function toggleMetric(metric) {
                // If humidity was active, clear it first
                activeMetrics.delete('humidity');

                if (activeMetrics.has(metric)) {
                    // Prevent deselecting if it's the only one left
                    if (activeMetrics.size > 1) {
                        activeMetrics.delete(metric);
                    }
                } else {
                    activeMetrics.add(metric);
                }

                updateCardStates();
                refreshChart();
            }

            // Selecting humidity clears temp & pressure
            function selectMetric(metric) {
                activeMetrics.clear();
                activeMetrics.add(metric);
                updateCardStates();
                refreshChart();
            }

            function renderHistoryList() {
                const listContainer = document.getElementById('history-list');
                const titleContainer = document.getElementById('history-title');
                if (!listContainer || !titleContainer) return;

                const metricsArr = [...activeMetrics];
                titleContainer.textContent = "Log " + metricsArr.map(m => m.charAt(0).toUpperCase() + m.slice(1)).join(' + ');
                listContainer.innerHTML = '';

                rawData.forEach((row, i) => {
                    let timestamp = parseInt(row['time']);
                    let timeStr = isNaN(timestamp) ? row['time'] : new Date((timestamp + 7200) * 1000).toISOString().substr(11, 8);

                    const item = document.createElement('div');
                    item.className = 'history-item';

                    let valuesHtml = metricsArr.map(metric => {
                        let val = row[metric];
                        let unit = configs[metric].unit;
                        let displayVal = val;
                        if (metric === 'temp') {
                            const kelvin = Math.round((parseFloat(val) + 273.15) * 100) / 100;
                            displayVal = `${kelvin}°K <small style="color:#888">(${val}°C)</small>`;
                            unit = '';
                        }
                        return `<span class="val" style="color:${configs[metric].color}">${displayVal}${unit ? ' ' + unit : ''}</span>`;
                    }).join(' | ');

                    item.innerHTML = `<span class="time">${timeStr}</span> : ${valuesHtml}`;
                    listContainer.appendChild(item);
                });
            }

            if (rawData.length > 0) {
                updateCardStates();
                refreshChart();
            }
        </script>
    </body>
</html>