// 1. Initialisation de la carte (centrée sur La Flèche)
const map = L.map('map').setView([47.698, -0.075], 11);

L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
    attribution: '© OpenStreetMap contributors'
}).addTo(map);

// 2. Dictionnaires pour stocker les marqueurs ET les tracés (lignes)
const markers = {};
const polylines = {};

// 3. Création des icônes textuelles/CSS sans emojis
const carIcon = L.divIcon({ html: '<div class="custom-label label-car">Voiture</div>', className: '', iconSize: [60, 24], iconAnchor: [30, 12] });
const balloonIcon = L.divIcon({ html: '<div class="custom-label label-balloon">Ballon</div>', className: '', iconSize: [60, 24], iconAnchor: [30, 24] });
const defaultIcon = L.divIcon({ html: '<div class="custom-label label-default">Inconnu</div>', className: '', iconSize: [60, 24], iconAnchor: [30, 24] });

// 4. Connexion WebSocket vers le serveur Qt
const ws = new WebSocket('ws://localhost:12345');

ws.onopen = () => {
    console.log("Connecté au serveur Qt avec succès !");
};

ws.onmessage = (event) => {
    try {
        const data = JSON.parse(event.data);

        if (data.type === 'position_update') {
            const callsign = data.name;
            const lat = data.lat;
            const lng = data.lng;

            // A. Définition de l'icône et de la couleur du tracé selon l'indicatif
            let currentIcon = defaultIcon;
            let lineColor = 'gray';

            if (callsign === 'F4KMN-10') {
                currentIcon = carIcon;
                lineColor = '#2196F3';
            } else if (callsign === 'F4KMN-9') {
                currentIcon = balloonIcon;
                lineColor = '#F44336';
            }

            // B. Préparation du contenu de la Popup (sans emojis)
            let popupContent = `<b>Véhicule : ${callsign}</b><br>Lat : ${lat.toFixed(5)}<br>Lng : ${lng.toFixed(5)}`;
            
            // Ajout de la télémétrie uniquement pour le ballon
            if (callsign === 'F4KMN-9') {
                const temp = data.temp !== undefined && data.temp !== "" ? data.temp : 'N/A';
                const press = data.pressure !== undefined && data.pressure !== "" ? data.pressure : 'N/A';
                const hum = data.humidity !== undefined && data.humidity !== "" ? data.humidity : 'N/A';
                const windDir = data.wind_direction !== undefined && data.wind_direction !== "" ? data.wind_direction : 'N/A';
                const windSpd = data.wind_speed !== undefined && data.wind_speed !== "" ? data.wind_speed : 'N/A';

                popupContent += `<hr style="margin:5px 0;">
                                 <b>Télémétrie :</b><br>
                                 Température : ${temp}°C<br>
                                 Pression : ${press} hPa<br>
                                 Humidité : ${hum}%<br>
                                 Vent : ${windSpd} km/h (Direction : ${windDir}°)`;
            }

            // C. Mise à jour de la carte (Marqueur + Tracé)
            if (!markers[callsign]) {
                markers[callsign] = L.marker([lat, lng], { icon: currentIcon })
                                     .addTo(map)
                                     .bindPopup(popupContent);
                
                polylines[callsign] = L.polyline([[lat, lng]], {
                    color: lineColor,
                    weight: 4,
                    opacity: 0.7
                }).addTo(map);

            } else {
                markers[callsign].setLatLng([lat, lng]);
                markers[callsign].setPopupContent(popupContent);
                polylines[callsign].addLatLng([lat, lng]);
            }
        }
    } catch (error) {
        console.error("Erreur lors de la lecture des données :", error);
    }
};

ws.onerror = (error) => {
    console.error("Erreur WebSocket :", error);
};

ws.onclose = () => {
    console.log("Connexion WebSocket fermée.");
};