// 1. Initialisation de la carte (centrée sur La Flèche)
const map = L.map('map').setView([47.698, -0.075], 11);

// Chargement des tuiles OpenStreetMap
L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
    attribution: '© OpenStreetMap contributors'
}).addTo(map);

// 2. Initialisation des marqueurs (ils sont à 0,0 au lancement)
let markerBallon = L.marker([0, 0]).addTo(map).bindPopup("Ballon Stratosphérique");
let markerVehicule = L.marker([0, 0]).addTo(map).bindPopup("Véhicule Suiveur");

// 3. Connexion WebSocket vers le serveur Qt
const ws = new WebSocket('ws://localhost:12345');

// Quand la connexion réussit
ws.onopen = () => {
    console.log("Connecté au serveur Qt avec succès !");
};

// Quand on reçoit un message du serveur Qt
ws.onmessage = (event) => {
    try {
        // On transforme le texte reçu en objet utilisable
        const data = JSON.parse(event.data);

        // On vérifie que c'est bien une mise à jour de position en direct
        if (data.type === 'position_update') {
            
            // On trie selon le nom (champ 'name' de ta table) et on utilise 'lat' et 'lng'
            if (data.name === 'F4GOH-10') {
                markerBallon.setLatLng([data.lat, data.lng]);
            } else if (data.name === 'F4IKQ-9') {
                markerVehicule.setLatLng([data.lat, data.lng]);
            }
        }
    } catch (error) {
        console.error("Erreur lors de la lecture des données :", error);
    }
};

// Gestion des erreurs de connexion
ws.onerror = (error) => {
    console.error("Erreur WebSocket (Le serveur Qt est-il allumé ?) :", error);
};

// Si le serveur Qt se coupe
ws.onclose = () => {
    console.log("Connexion WebSocket fermée.");
};