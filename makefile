all:
	g++ -std=c++11 main.cpp Jogador.cpp PlayerAnimation.cpp LifeMushroom.cpp Flower.cpp Background.cpp -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -o main

