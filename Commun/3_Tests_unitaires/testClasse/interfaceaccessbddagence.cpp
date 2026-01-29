#include "interfaceaccessbddagence.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QJsonObject>
#include <QSettings>
/**
 * @brief InterfaceAccessBddAgence::InterfaceAccessBddAgence
*
* Initialise la connexion à la base de données
* @param parent
 */
InterfaceAccessBddAgence::InterfaceAccessBddAgence(QObject *parent)
    : QObject{parent}
{
    chargerParametresBdd();
    bdd = QSqlDatabase::addDatabase("QMYSQL");
    bdd.setHostName(adresse);
    bdd.setDatabaseName(nomBase);
    bdd.setUserName(login);
    bdd.setPassword(motDePasse);
    bool ok = bdd.open();
    if (ok)
    {
        qDebug()<<"ouverture ok";

    }
    else
    {
        qDebug()<<bdd.lastError();
    }
}

/**
 * @brief InterfaceAccessBddAgence::ajouterReservation
 *
 * Ajoute la réservation dans la table des réservation
 * @param ref référence du vol
 * @param idClient id du client
 * @param nroSiege numéro du siège réservé

 *
 * @return true si reservation ok, false sinon
 */

bool InterfaceAccessBddAgence::ajouterReservation(int ref, int idClient, int nroSiege)
{
    bool resaOk=true;
    QSqlQuery requetePreparee;
    requetePreparee.prepare("insert into reservations (referenceVol,idClient,numeroSiege) values (:ref,:id,:nro); ");
    requetePreparee.bindValue(":ref",ref);
    requetePreparee.bindValue(":id",idClient);
    requetePreparee.bindValue(":nro",nroSiege);
    if (!requetePreparee.exec())
    {
        qDebug()<<requetePreparee.lastError().text();
        resaOk=false;
    }
    return resaOk;
}

/**
 * @brief InterfaceAccessBddAgence::supprimerReservation
 *
 * Supprimer la réservation ayant pour id idResa de la table reservations
 * @param idResa id de la réservation à supprimer
 * @return true si la réservation à bien été supprimée false sinon
 */
bool InterfaceAccessBddAgence::supprimerReservation(int idResa)
{
    bool supOk=true;
    QSqlQuery requetePreparee;
    requetePreparee.prepare("delete from reservations where idResa=:id; ");
    requetePreparee.bindValue(":id",idResa);

    if (!requetePreparee.exec())
    {
        qDebug()<<requetePreparee.lastError().text();
        supOk=false;
    }
    return supOk;
}


/**
 * @brief obtenirListeDesPlacesOccupees
 *
 * retourne la liste des numéros de siege déjà réservé pour le vol refVol
 * @param refVol référence du vol pour lequel on souhaite la liste des places non disponibles.
 *
 * @return liste des numéros de siège
 */
QList <int> obtenirListeDesPlacesOccupees(int refVol)
{
    QList <int>places;
    QSqlQuery requetePreparee;
    requetePreparee.prepare("select numeroSiege from reservations where referenceVol=:ref; ");
    requetePreparee.bindValue(":ref",refVol);

    if (!requetePreparee.exec())
    {
        qDebug()<<requetePreparee.lastError().text();
    }
    else
    {
        while(requetePreparee.next())
        {
            places.append(requetePreparee.value("numeroSiege").toInt());
        }
    }
    return places;
}

/**
 * @brief InterfaceAccessBddAgence::obtenirListeDesVols
 *
 * Génère la liste des vols en fonctions des données présentes dans la table vols de la base de données.
 * @return liste des vols
 */
QList<avion *> InterfaceAccessBddAgence::obtenirListeDesVols()
{
    QList<avion *>lesVols;
    QSqlQuery requete("select reference,denomination from vols order by reference; ");
    while(requete.next())
    {
        avion *a=new avion;
        a->infosVol.reference=requete.value("reference").toInt();
        a->infosVol.denomination=requete.value("denomination").toString();
        a->siegesOccupees=obtenirListeDesPlacesOccupees(a->infosVol.reference);
        lesVols.append(a);
    }
    return lesVols;
}


/**
 * @brief InterfaceAccessBddAgence::chargerParametresBdd
 *
 * initialise les attributs de la classes en fonctions des données du fichier ServeurAgenceBdd.ini
 */
void InterfaceAccessBddAgence::chargerParametresBdd()
{
    QSettings param("ServeurAgenceBdd.ini",QSettings::IniFormat);
    login=param.value("BDD/login","snir").toString();
    motDePasse=param.value("BDD/motDePasse","snir").toString();
    adresse=param.value("BDD/adresseIp","172.18.58.7").toString();
    nomBase=param.value("BDD/nomDeLaBase","agence").toString();

}

/**
 * @brief InterfaceAccessBddAgence::ajouterClient
 *
 * Ajouter un client à la table clients. Si le client existe déjà il n'est pas ajouté.
 * @param nom
 * @param prenom
 * @param email
 * @return id du client ajouté à la table clients
 */
int InterfaceAccessBddAgence::ajouterClient(QString nom, QString prenom, QString email)
{
    QSqlQuery requetePreparee;
    int idClient=-1;
    // rechercher l'id du client selon son email
    requetePreparee.prepare("select id from clients where email like :email; ");
    requetePreparee.bindValue(":email",email);
    if (!requetePreparee.exec())
    {
        qDebug()<<requetePreparee.lastError().text();
    }
    else
    {
        // si le client existe, récupérer son id
        if (requetePreparee.size()!=0)
        {
            requetePreparee.next();
            idClient=requetePreparee.value("id").toInt();
        }
        else    // si le client n'existe pas, le créer et récupérer son id
        {
            QSqlQuery ajoutClient;
            ajoutClient.prepare("insert into clients (nom, prenom, email)  values (:n,:p,:e);");
            ajoutClient.bindValue(":n",nom);
            ajoutClient.bindValue(":p",prenom);
            ajoutClient.bindValue(":e",email);
            if (!ajoutClient.exec())
            {
                qDebug()<<ajoutClient.lastError().text();
            }
            else
            {
                idClient=ajoutClient.lastInsertId().toInt();
            }

        }
    }
    return idClient;

}
/**
 * @brief InterfaceAccessBddAgence::obtenirListeReservations
 *
 * récupère l'id de la réservation, la référence du vol, le nom du vol, le numéro du siège, le nom, le prénom et l'email du client
 * pour toutes les réservations
 * @return objet de type tableau json contenant les informations des réservation
 */
QJsonArray InterfaceAccessBddAgence::obtenirListeReservations()
{
    QJsonArray listeResa;
    QSqlQuery requete("select vols.reference,vols.denomination, reservations.idResa,reservations.numeroSiege, clients.nom, clients.prenom, clients.email    "
                      "from vols, reservations,clients    "
                      "where vols.reference=reservations.referenceVol    "
                      "and clients.id=reservations.idClient    "
                      "order by vols.reference , reservations.numeroSiege;");
    while(requete.next())
    {
        QJsonObject resa;
        resa["idResa"]=requete.value("idResa").toInt();
        resa["ref"]=requete.value("reference").toInt();
        resa["vol"]=requete.value("denomination").toString();
        resa["siege"]=requete.value("numeroSiege").toInt();
        resa["nom"]=requete.value("nom").toString();
        resa["prenom"]=requete.value("prenom").toString();
        resa["email"]=requete.value("email").toString();
        listeResa.append(resa);
    }
    return listeResa;
}

/**
 * @brief InterfaceAccessBddAgence::ajouterVol
 *
 * Ajoute un nouveau vol à la table vols
 * @param vol nom du nouveau vol à ajouter
 * @return l'id du vol créé ou -1 en cas d'erreur.
 */

int InterfaceAccessBddAgence::ajouterVol(QString vol)
{
    int idVol=-1;
    QSqlQuery ajoutVol;
    ajoutVol.prepare("insert into vols (denomination)  values (:n);");
    ajoutVol.bindValue(":n",vol);

    if (!ajoutVol.exec())
    {
        qDebug()<<ajoutVol.lastError().text();

    }
    else
    {
        idVol=ajoutVol.lastInsertId().toInt();
    }
    return idVol;

}
