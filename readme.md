projet architecture des ordinateur | maison connect


TODO:

Plusieurs arduino recepteur/emetteur
Un seul arduino serveur recepteur (ou rasberry)


construction d'une trame depuis un arduino capteur:
ID_Trame:ID_arduino:temperature:humidité:présence:lumiere:

construction d'une trame depuis un arduino serveur :
ID_serveur:ID_destinataire:action
id_server doit etre reconnu par tous, par exemple 00

Pour les arduinos : 
    - Si ils reçoivent une trame, ils la renvoient (sauf s'ils l'ont déjà envoyé auparavant, pour éviter les boucles)

Pour le serveur : 
    - Si ils reçoivent une trame :
        - Soit ils ne l'ont pas déjà reçu, et la traite
        - Soit ils l'ont déjà reçu, et l'ignorent.