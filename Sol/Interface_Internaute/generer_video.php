<?php
/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Scripting/EmptyPHP.php to edit this template
 */

// 1. Connexion à la BDD
$host = '172.18.58.85';
$dbname = 'ballon2026';
$user = 'root';
$pass = 'toto';

try {
    $db = new PDO("mysql:host=$host;dbname=$dbname", $user, $pass);
} catch (Exception $e) {
    die('Erreur : ' . $e->getMessage());
}

// 2. Récupérer les chemins des images présentes en BDD
$query = $db->query("SELECT chemin_image FROM IMAGE ORDER BY id_image ASC");
$images = $query->fetchAll(PDO::FETCH_COLUMN);

$nom_video = "timelapse_sstv.mp4";
$output = "";
$videoGeneree = false;
$debugTxt = "";

if (count($images) > 0) {
    // 3. Créer un fichier texte temporaire pour FFmpeg (concat demuxer)
    $fileList = "images_to_video.txt";
    $content = "";
    foreach ($images as $img) {
        // On prend juste le nom du fichier pour éviter les erreurs de chemin stockés en BDD
        $nomFichier = basename($img);
        $path = "/home/USERS/ELEVES/CIEL2024/hkalo/sstvYoniq/ARCHIVE_PHOTOS/" . $nomFichier;

        // On écrit le fichier dans la liste
        $content .= "file '$path'\nduration 1\n";
    }
    // Il faut répétér la dernière image pour FFmpeg
    if ($content != "") {
        $content .= "file '$path'";
    }
    file_put_contents($fileList, $content);
    
    // On mémorise proprement le contenu pour l'afficher plus bas dans le HTML
    $debugTxt = file_get_contents($fileList);

    // 4. Lancer FFmpeg avec la liste précise
    if (file_exists($nom_video)) {
        unlink($nom_video);
    }

    $commande = "ffmpeg -y -f concat -safe 0 -i $fileList -c:v libx264 -pix_fmt yuv420p $nom_video 2>&1";
    $output = shell_exec($commande);

    if (file_exists($nom_video)) {
        $videoGeneree = true;
    }
}
?>

<!DOCTYPE html>
<html lang="fr">
    <head>
        <meta charset="UTF-8">
        <title>Génération Vidéo SSTV</title>
        <link rel="stylesheet" href="style.css">
        <style>
            /* Conteneur principal harmonisé */
            .video-main-container {
                max-width: 800px;
                margin: 50px auto;
                background: #1e1e1e;
                padding: 30px;
                border-radius: 15px;
                border: 1px solid #90a4ae; /* Bordure assortie gris-bleuté */
                box-shadow: 0 10px 30px rgba(0,0,0,0.5);
            }
            video {
                border-radius: 10px;
                border: 2px solid #333;
                width: 100%;
                margin: 20px 0;
                box-shadow: 0 5px 15px rgba(0,0,0,0.3);
            }

            /* Base commune pour les nouveaux boutons modernes */
            .btn-action {
                display: inline-block;
                padding: 12px 28px;
                font-size: 1rem;
                font-weight: bold;
                text-decoration: none;
                border-radius: 25px; /* Coins bien arrondis */
                transition: all 0.2s ease-in-out;
                cursor: pointer;
            }

            /* Bouton Retour : Blanc cassé / Argenté / Gris-Bleuté */
            .btn-return {
                background: #90a4ae;
                color: #1a1a1a;
                box-shadow: 0 4px 10px rgba(144, 164, 174, 0.2);
            }
            .btn-return:hover {
                background: #b0bec5;
                transform: translateY(-2px);
                box-shadow: 0 6px 15px rgba(144, 164, 174, 0.4);
            }

            /* Bouton Actualiser : Bleu moderne et dynamique */
            .btn-refresh {
                background: #3498db;
                color: white;
                box-shadow: 0 4px 10px rgba(52, 152, 219, 0.2);
            }
            .btn-refresh:hover {
                background: #2980b9;
                transform: translateY(-2px);
                box-shadow: 0 6px 15px rgba(52, 152, 219, 0.4);
            }

            .debug-box {
                text-align: left;
                font-size: 11px;
                color: #f39c12;
                background: #111;
                padding: 15px;
                border-radius: 8px;
                max-height: 180px;
                overflow-y: auto;
                margin-top: 25px;
                border: 1px solid #222;
                font-family: monospace;
            }
        </style>
    </head>
    <body>
        <h1>Synthèse Vidéo</h1>

        <div class="video-main-container">
            <?php if (!$videoGeneree): ?>
                <h2 style="color: #f1c40f;">Aucune vidéo</h2>
                <p>Soit la BDD est vide, soit les fichiers sont introuvables.</p>
                <pre style="text-align:left; font-size:11px; color:#e74c3c; background:#111; padding:15px; border-radius:8px; overflow-x:auto;"><?php echo htmlspecialchars($output); ?></pre>
                
                <div style="display: flex; justify-content: center; gap: 15px; margin-top: 25px;">
                    <a href="index.php" class="btn-action btn-return">Retour</a>
                    <a href="generer_video.php" class="btn-action btn-refresh">Actualiser la vidéo</a>
                </div>
            <?php else: ?>
                <h2 style="color: #90a4ae;">Timelapse mis à jour</h2>
                <p style="color: #bbb;">Images en base : <?php echo count($images); ?></p>

                <video controls autoplay>
                    <source src="<?php echo $nom_video; ?>?t=<?php echo time(); ?>" type="video/mp4">
                </video>

                <div style="display: flex; justify-content: center; gap: 15px; margin-top: 25px;">
                    <a href="index.php" class="btn-action btn-return">Retour</a>
                    <a href="generer_video.php" class="btn-action btn-refresh">Actualiser la vidéo</a>
                </div>
            <?php endif; ?>

            <?php if (!empty($debugTxt)): ?>
                <div class="debug-box">
                    <strong style="color: #eee;">Contenu du fichier texte FFmpeg :</strong><br>
                    <?php echo htmlspecialchars($debugTxt); ?>
                </div>
            <?php endif; ?>
        </div>
    </body>
</html>