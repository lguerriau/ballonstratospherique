// 1. Initialisation de la carte (centrée sur La Flèche)
const map = L.map('map').setView([47.698, -0.075], 11);

// Chargement des tuiles OpenStreetMap
L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
    attribution: '© OpenStreetMap contributors'
}).addTo(map);

// 2. Dictionnaire pour stocker les marqueurs dynamiquement
// Clé = Nom (callsign), Valeur = Objet Marqueur Leaflet
const markers = {};

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
            const callsign = data.name;
            const lat = data.lat;
            const lng = data.lng;

            // Si le marqueur n'existe pas encore pour ce véhicule, on le crée
            if (!markers[callsign]) {
                markers[callsign] = L.marker([lat, lng])
                                     .addTo(map)
                                     .bindPopup("Véhicule : " + callsign);
            } else {
                // S'il existe déjà, on le déplace simplement vers ses nouvelles coordonnées
                markers[callsign].setLatLng([lat, lng]);
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