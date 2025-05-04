projet architecture des ordinateur | maison connect

   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
/!\ - - penser à inclure les librairie dans l'IDE Arduino - - /!\
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
   
- emetteur/emet/emet.ino -> code de l'émetteur

- recepteur/tp6-liaison-RF-recepteur/tp6-liaison-RF-recepteur.ino -> code du recepteur (Serveur)

   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

TODO:

Plusieurs arduino recepteur/emetteur
Un seul arduino serveur recepteur (ou rasberry)


construction d'une trame depuis un arduino capteur:
XX:ID_Trame:ID_arduino:temperature:humidité:présence:lumiere:
XX:ID_Trame:ID_arduino::

construction d'une trame depuis un arduino serveur :
XX:ID_serveur:ID_destinataire:action
XX:id_server doit etre reconnu par tous, par exemple 00

Pour les arduinos : 
    - Si ils reçoivent une trame, ils la renvoient (sauf s'ils l'ont déjà envoyé auparavant, pour éviter les boucles)

Pour le serveur : 
    - Si ils reçoivent une trame :
        - Soit ils ne l'ont pas déjà reçu, et la traite
        - Soit ils l'ont déjà reçu, et l'ignorent.
    - Faire un menu qui s'affiche toutes les 3secondes par exemple, et qui montre les dernières (ou moyenne) infos reçues

nettoyer le code du serveur
