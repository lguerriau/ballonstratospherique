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

if (count($images) > 0) {
    // 3. Créer un fichier texte temporaire pour FFmpeg (concat demuxer)
    $fileList = "images_to_video.txt";
    $content = "";
    foreach ($images as $img) {
        // On prend juste le nom du fichier pour éviter les erreurs de chemin stockés en BDD
        $nomFichier = basename($img);
        $path = "/home/USERS/ELEVES/CIEL2024/hkalo/sstvYoniq/ARCHIVE_PHOTOS/" . $nomFichier;

        // On écrit le fichier dans la liste même si PHP a un doute sur l'existence
        $content .= "file '$path'\nduration 1\n";
    }
// Il faut répéter la dernière image ou ajouter une durée finale pour FFmpeg
    if ($content != "") {
        $content .= "file '$path'";
    }
    file_put_contents($fileList, $content);
    echo "<pre style='color:orange;'>Contenu du fichier texte : \n" . file_get_contents($fileList) . "</pre>";

    // 4. Lancer FFmpeg avec la liste précise
    // On force la suppression de l'ancien fichier
    if (file_exists($nom_video)) {
        unlink($nom_video);
    }

    $commande = "ffmpeg -y -f concat -safe 0 -i $fileList -c:v libx264 -pix_fmt yuv420p $nom_video 2>&1";
    $output = shell_exec($commande);
    sleep(2);

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
            body {
                background: #1a1a1a;
                color: white;
                font-family: sans-serif;
                text-align: center;
            }
            .video-main-container {
                max-width: 800px;
                margin: 50px auto;
                background: #1e1e1e;
                padding: 30px;
                border-radius: 15px;
                border: 1px solid #e74c3c;
                box-shadow: 0 10px 30px rgba(0,0,0,0.5);
            }
            video {
                border-radius: 10px;
                border: 2px solid #333;
                width: 100%;
                margin: 20px 0;
            }
            .btn-refresh {
                background: #3498db !important;
            }
        </style>
    </head>
    <body>
        <h1>🎬 Synthèse Vidéo</h1>

        <div class="video-main-container">
            <?php if (!$videoGeneree): ?>
                <h2 style="color: #f1c40f;">AUCUNE VIDÉO</h2>
                <p>Soit la BDD est vide, soit les fichiers sont introuvables.</p>
                <pre style="text-align:left; font-size:10px; color:red;"><?php echo $output; ?></pre>
                <a href="index.php" class="btn">⬅ RETOUR</a>
            <?php else: ?>
                <h2 style="color: #e74c3c;">TIMELAPSE MIS À JOUR</h2>
                <p style="color: #bbb;">Images en base : <?php echo count($images); ?></p>

                <video controls autoplay>
                    <source src="<?php echo $nom_video; ?>?t=<?php echo time(); ?>" type="video/mp4">
                </video>

                <div style="display: flex; justify-content: center; gap: 10px;">
                    <a href="index.php" class="btn">⬅ RETOUR</a>
                    <a href="generer_video.php" class="btn btn-refresh">🔄 ACTUALISER LA VIDÉO</a>
                </div>
            <?php endif; ?>
        </div>
    </body>
</html>