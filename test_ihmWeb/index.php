<?php
/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Scripting/EmptyPHP.php to edit this template
 */

// Connexion à la base de données
$host = '172.18.58.85';
$dbname = 'ballon2026';
$user = 'root';
$pass = 'toto';

try {
    $db = new PDO("mysql:host=$host;dbname=$dbname", $user, $pass);
} catch (Exception $e) {
    die('Erreur : ' . $e->getMessage());
}

// Récupérer les images (on utilise le chemin stocké en BDD)
$requete = $db->query('SELECT id_image, chemin_image, horodatage_image FROM IMAGES ORDER BY id_image DESC');
?>

<!DOCTYPE html>
<html lang="fr">
    <head>
        <meta charset="UTF-8">
        <title>SSTV Mission Control</title>
        <link rel="stylesheet" href="style.css">
    </head>
    <body>
        <h1>🛰️ Station de Réception SSTV</h1>

        <div class="actions">
            <a href="generer_video.php" class="btn">🎬 Générer la Vidéo</a>
        </div>

        <div class="galerie">
            <?php while ($donnees = $requete->fetch()): ?>
                <div class="photo-card">
                    <!-- On affiche l'image en utilisant le nom du fichier stocké en BDD -->
                    <img src="photos_sstv/<?php echo basename($donnees['chemin_image']); ?>" alt="SSTV">
                    <p>Photo n°<?php echo $donnees['id_image']; ?></p>
                    <p>Reçue à : <?php echo $donnees['horodatage_image']; ?></p>
                </div>
            <?php endwhile; ?>
        </div>
        <div class="photo-card">
            <img src="photos_sstv/<?php echo basename($donnees['chemin_image']); ?>" alt="SSTV">
            <p>SÉQUENCE #<?php echo $donnees['id_image']; ?></p>
            <span>📡 Reçu à : <?php echo date('H:i:s', strtotime($donnees['horodatage_image'])); ?></span>
        </div>
    </body>
</html>