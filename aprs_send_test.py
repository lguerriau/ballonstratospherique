import socket
import time

# Configuration APRS-IS
APRS_SERVER = "euro.aprs2.net"
APRS_PORT = 14580
TOOL_NAME = "ESP32-APRS-IS"
VERSIONIS = "1.1"
USER = "F4KMN"
PASSCODE = "12443"

# Constantes
MAX_DEGREES = 180
MAX_MINUTES = 60.0
MIN_MINUTES = 0.0
LONGITUDE_INCREMENT = 0.05
MAX_RETRIES = 3
SOCKET_TIMEOUT = 10
BUFFER_SIZE = 4096

# Variable globale pour la longitude
current_longitude = "00017.61E"

def validate_longitude(lon_str):
    """
    Valide le format de la longitude APRS.
    Retourne True si valide, False sinon.
    """
    if len(lon_str) != 9:
        return False
    
    hemi = lon_str[-1]
    if hemi not in ['E', 'W']:
        return False
    
    try:
        num_part = lon_str[:-1]
        degrees = int(num_part[:3])
        minutes = float(num_part[3:])
        
        if degrees < 0 or degrees >= MAX_DEGREES:
            return False
        if minutes < MIN_MINUTES or minutes >= MAX_MINUTES:
            return False
            
        return True
    except (ValueError, IndexError):
        return False

def increment_longitude(lon_str, increment):
    """
    Incrémente une longitude au format APRS (dddmm.mmH)
    Retourne la nouvelle longitude ou None en cas d'erreur.
    """
    if not validate_longitude(lon_str):
        print(f"ERREUR: Longitude invalide: {lon_str}")
        return None
    
    if increment <= 0 or increment >= MAX_MINUTES:
        print(f"ERREUR: Incrément invalide: {increment}")
        return None
    
    # Extraire l'hémisphère
    hemi = lon_str[-1]
    num_part = lon_str[:-1]
    
    # Séparer degrés et minutes
    degrees = int(num_part[:3])
    minutes = float(num_part[3:])
    
    # Incrémenter
    minutes = minutes + increment
    
    # Gérer le dépassement des minutes
    if minutes >= MAX_MINUTES:
        minutes = minutes - MAX_MINUTES
        degrees = degrees + 1
        
        # Vérifier le débordement des degrés
        if degrees >= MAX_DEGREES:
            print(f"ERREUR: Débordement des degrés: {degrees}")
            return None
    
    # Reformater avec le bon padding
    new_lon = f"{degrees:03d}{minutes:05.2f}{hemi}"
    
    # Valider avant de retourner
    if not validate_longitude(new_lon):
        print(f"ERREUR: Longitude générée invalide: {new_lon}")
        return None
    
    return new_lon


def connect_aprs():
    """
    Établit la connexion initiale avec le serveur APRS-IS.
    Retourne l'objet socket si succès, None sinon.
    """
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(SOCKET_TIMEOUT)
        
        print(f"Connexion à {APRS_SERVER}:{APRS_PORT}...")
        sock.connect((APRS_SERVER, APRS_PORT))
        print("Connecté !")
        
        # Lire le message de bienvenue
        try:
            welcome = sock.recv(BUFFER_SIZE).decode(errors="ignore")
            print("Serveur:", welcome.strip())
        except socket.timeout:
            print("Timeout lecture bienvenue")
        
        # Authentification
        login = (
            f"user {USER} "
            f"pass {PASSCODE} "
            f"vers {TOOL_NAME} {VERSIONIS}\n\r"
        )
        print("Envoi login :", login.strip())
        sock.sendall(login.encode())
        
        time.sleep(1)
        
        # Lire réponse du serveur
        try:
            response = sock.recv(BUFFER_SIZE).decode(errors="ignore")
            print("Réponse serveur:", response.strip())
        except socket.timeout:
            print("Timeout lecture réponse")
            
        return sock
        
    except Exception as e:
        print(f"ERREUR lors de la connexion initiale: {e}")
        if sock is not None:
            sock.close()
        return None


def send_aprs(sock):
    """
    Envoie un message APRS-IS via la connexion persistante.
    Retourne True si succès, False sinon.
    """
    global current_longitude
    
    if not validate_longitude(current_longitude):
        print("ERREUR: Longitude courante invalide avant envoi")
        return False
    
    try:
        # Message APRS avec la longitude actuelle
        aprs_message = f"F4KMN-10>APRS,WIDE1-1:!4753.41N/{current_longitude}>\n\r"
        print(f"Envoi trame APRS : {aprs_message.strip()}")
        print(f"Longitude actuelle : {current_longitude}")
        
        sock.sendall(aprs_message.encode())
        print("Message envoyé avec succès.")
        
        # On lit brièvement s'il y a du trafic réseau retour
        # Le timeout court évitera de bloquer la boucle s'il n'y a rien
        sock.settimeout(2.0) 
        try:
            data = sock.recv(BUFFER_SIZE).decode(errors="ignore")
            if data:
                print("Données reçues:")
                print(data)
        except socket.timeout:
            # Très fréquent en mode persistant (le serveur ne répond pas à chaque trame)
            pass
        finally:
            # Remettre le timeout normal pour le prochain tour
            sock.settimeout(SOCKET_TIMEOUT)
            
        return True
        
    except socket.error as e:
        print(f"ERREUR socket (déconnexion ?): {e}")
        return False
    except Exception as e:
        print(f"ERREUR inattendue: {e}")
        return False


def update_longitude():
    """
    Met à jour la longitude globale.
    Retourne True si succès, False sinon.
    """
    global current_longitude
    
    new_longitude = increment_longitude(current_longitude, LONGITUDE_INCREMENT)
    
    if new_longitude is None:
        print("ERREUR: Impossible d'incrémenter la longitude")
        return False
    
    print(f"Prochaine longitude : {new_longitude}")
    current_longitude = new_longitude
    return True


def run_tests():
    """
    Exécute les tests de validation.
    """
    print("=== Tests de validation ===")
    
    # Test validation
    test_valid = ["00017.61E", "00059.99W", "17959.99E"]
    for val in test_valid:
        result = validate_longitude(val)
        print(f"validate_longitude('{val}') = {result}")
    
    print()
    
    # Test incrémentation
    print("=== Test d'incrémentation (+0.05) ===")
    test_values = ["00017.61E", "00017.95E", "00059.95E", "00059.99E"]
    
    max_iterations = len(test_values)
    iteration_count = 0
    
    for val in test_values:
        if iteration_count >= max_iterations:
            break
        result = increment_longitude(val, LONGITUDE_INCREMENT)
        if result is not None:
            print(f"{val} + {LONGITUDE_INCREMENT} = {result}")
        else:
            print(f"{val} + {LONGITUDE_INCREMENT} = ERREUR")
        iteration_count = iteration_count + 1
    
    print()


def main():
    """
    Fonction principale.
    """
    run_tests()
    
    # 1. Établissement de la connexion en amont de la boucle
    sock = connect_aprs()
    if sock is None:
        print("Impossible de se connecter. Arrêt du programme.")
        return
    
    send_count = 0
    
    try:
        # 2. Boucle infinie d'envoi sur la même connexion
        while True:
            print(f"\n=== Envoi #{send_count + 1} ===")
            
            # Envoyer le message APRS
            send_success = send_aprs(sock)
            
            if not send_success:
                print("ERREUR: Échec de l'envoi APRS. Le socket est peut-être fermé.")
                break # On sort de la boucle si la connexion est perdue
            
            # Mettre à jour la longitude
            update_success = update_longitude()
            
            if not update_success:
                print("ERREUR: Échec de la mise à jour de la longitude")
                break
            
            # Pause de 30 secondes
            time.sleep(30)
            send_count = send_count + 1
            
    finally:
        # 3. Fermeture propre à la toute fin du programme (ex: si interrompu par Ctrl+C)
        if sock is not None:
            try:
                sock.close()
                print("\nConnexion fermée proprement.")
            except Exception as e:
                print(f"ERREUR fermeture socket: {e}")
    
    print("=== Fin du programme ===")


if __name__ == "__main__":
    main()