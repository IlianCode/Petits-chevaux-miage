# PROJET PETITS CHEVAUX MIAGE ***(english readme below)***  </br> 
</br>
Dans le cadre du cours de Systèmes de la L3 MIAGE de grenoble, nous avons dû créer un jeu en <b>C</b> reprenant les règles du célèbre <b>jeu des petits chevaux</b> .</br>
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
</br>
</br>

# English readme
# Little horses game / Ludo  Project

As part of the Systems course at L3 MIAGE in Grenoble, we had to create a game in ***C*** using the rules of the famous ***little horses game***.
The purpose of this project is to introduce us to the creation and management of fork processes and to understand how to communicate between the different sub-processes.
The game having can take up to 4 potential players, we will then have to dynamically create 4 processes and 5 tubes (the management of the number of players is done in CLI). </br>

## Project Concept Diagram: </br>

![alt text](https://github.com/IlianCode/Petits-chevaux-miage/blob/main/jeu/schema-conception.png)

</br>

## Compiling and running the project : </br>

A makefile is present in the project, to launch it, all you have to do is open a terminal, place it in the right directory and type the following commands:
</br>
```
make
```
then : 
```
./main 4
```

### Notes : 
Bugs exist when choosing the final winner, but the lack of time to complete this project has prevented me from fixing them for the moment. </br>

[Rules of the game: ](https://www.regledujeu.fr/petits-chevaux/)


