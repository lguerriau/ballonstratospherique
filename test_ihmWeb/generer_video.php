<?php

/* 
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Scripting/EmptyPHP.php to edit this template
 */

$dossier_images = "/home/USERS/ELEVES/CIEL2024/hkalo/sstvYoniq/ARCHIVE_PHOTOS";
$nom_video = "timelapse_sstv.mp4";

echo "<h2>Génération de la vidéo en cours...</h2>";

// Commande FFmpeg :
// -framerate 2 : 2 images par seconde
// -pattern_type glob -i '*.jpg' : prend tous les fichiers JPG
// -y : écrase la vidéo si elle existe déjà
$commande = "ffmpeg -y -framerate 2 -pattern_type glob -i '$dossier_images/*.bmp' -c:v libx264 -pix_fmt yuv420p timelapse_sstv.mp4 2>&1";
$resultat = shell_exec($cmd);
// Exécution de la commande
$output = shell_exec($commande);

// Affichage du résultat
if (file_exists($nom_video)) {
    echo "<p>Succès ! La vidéo est prête.</p>";
    echo "<video width='640' controls><source src='$nom_video' type='video/mp4'></video>";
    echo "<br><a href='index.php'>Retour à la galerie</a>";
} else {
    echo "<p>Erreur lors de la génération :</p><pre>$output</pre>";
}
?>