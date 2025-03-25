projet architecture des ordinateur | maison connect


TODO:

Plusieurs arduino recepteur/emetteur
Un seul arduino serveur recepteur (ou rasberry)


construction d'une trame :
ID_Trame.


Pour les arduinos : 
    - Si ils reçoivent une trame, ils la renvoient (sauf s'ils l'ont déjà envoyé auparavent, pour éviter les boucles)

Pour le serveur : 
    - Si ils reçoivent une trame :
        - Soit ils ne l'ont pas déjà reçu, et la traite
        - Soit ils l'ont déjà reçu, et l'ignorent.