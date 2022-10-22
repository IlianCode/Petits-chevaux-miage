# PROJET PETITS CHEVAUX MIAGE </br> 
Dans le cadre du cours de Systèmes de la L3 MIAGE de grenoble, nous avons dû créer un jeu en **C** reprenant les règles du célèbre **jeu des petits chevaux**.</br>
L’objectif de ce projet est de nous introduire à la création et gestion des processus et de comprendre comment communiquer entre les différents sous processus.</br>
Le jeu devant prendre en compte jusqu’à 4 joueurs potentiels, il nous faudra alors créer dynamiquement 4 processus et 5 tubes (la gestion du nombre de joueurs se faisant au moment de l’exécution du programme en ligne de commande).</br> 
</br>
## Schéma de conceptualisation du projet : </br>
![alt text](https://github.com/IlianCode/Petits-chevaux-miage/blob/main/jeu/schema-conception.png)

</br>
## Compilation et exécution du projet </br>

Un makefile est présent dans le projet, pour le lancer il suffit donc d'ouvrir un terminal, de ce placer dans le repertoire et de taper les commandes suivantes : </br>
```
make 
```
puis : 
```
./main 4
```
</br>

### Remarques : 
Des bugs existent au moment de choisir le gagnant final, mais le manque de temps pour réaliser ce projet m'as empéché de les corriger pour le moment.</br>

[Les règles du jeu ici.](https://www.regledujeu.fr/petits-chevaux/)
