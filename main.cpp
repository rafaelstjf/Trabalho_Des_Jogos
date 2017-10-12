#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#ifdef __linux__
#include <cstring>
#endif
//SFML Libraries
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

//Game Libraries
#include "Player.h"
#include "SuperMushroom.h"
#include "Flower.h"
#include "Platform.h"
#include "Coin.h"
#include "HUD.h"
#include "SoundManager.h"
#include "QuestionBlock.h"
namespace patch //Correcao da funcao to_string
{
template <typename T>
std::string to_string(const T &n)
{
    std::ostringstream stm;
    stm << n;
    return stm.str();
}
}
using namespace std;

static const string coinTPath = "Textures/General/Coin.png";
static const string sMushroomTPath = "Textures/General/SuperMushroom.png";
static const string groundTPath = "Textures/Background/Ground.png";
static const string logoTPath = "Textures/General/logo.png";
static const string transparentTPath = "Textures/Background/Transparent.png";
static const string tube2TPath = "Textures/Background/Tube2.png";
static const string tube1TPath = "Textures/Background/Tube1.png";

const float switchTime = 0.1f;
static const float VIEW_HEIGHT = 600.0f;
static const float VIEW_WIDTH = 800.0f;

void resizeWindow(sf::RenderWindow &window, sf::View &view)
{
    float aspectRatio = float(window.getSize().x) / float(window.getSize().y);
    view.setSize(VIEW_WIDTH * aspectRatio, VIEW_HEIGHT);
}

int main()
{
    //Inicializa as variaveis
    float deltaTime = 0.0f;
    bool gameOpen = true;
    bool inMenu = true;
    sf::Clock clock;
    sf::Vector2f direction;
    int pontuacao = 0;
    int vidas = 0;
    //Cria a window e a view
    sf::RenderWindow game;
    sf::RenderWindow menu;
    //Carrega a fonte
    sf::Font font;
    font.loadFromFile("Extra/font.ttf");
    //Carrega a textura de alguns objetos
    sf::Texture coinTexture;
    sf::Texture transparentTexture;
    transparentTexture.loadFromFile(transparentTPath);
    coinTexture.loadFromFile(coinTPath);
    sf::Texture sMushroomTexture;
    sf::Texture groundTexture;
    sf::Texture tube2Texture;
    tube2Texture.loadFromFile(tube2TPath);
    groundTexture.loadFromFile(groundTPath);
    sMushroomTexture.loadFromFile(sMushroomTPath);



    //Inicializa os objetos
    SoundManager sound;
    Player mario(100.0f, switchTime, 100, sf::Vector2f(16.0f,300.0f));
    vector <Coin> coins;
    vector<Platform> platforms;

    vector <SuperMushroom> smushrooms;
    platforms.push_back( Platform(groundTexture, sf::Vector2f(4800.0f,32.0f), sf::Vector2f(2400,600-16) ) );
    platforms.push_back( Platform(transparentTexture, sf::Vector2f(32.0f, 600.0f), sf::Vector2f(-16.0f, 300.0f) ) );
    platforms.push_back( Platform(tube2Texture, sf::Vector2f(64.0f, 96.0f), sf::Vector2f(300.0f, 600-79.0f) ) );

    HUD hud;
    //

    //executa uma sequencia de eventos enquanto a janela esta aberta
    while(gameOpen)
    {
        if(inMenu)
        {
            menu.create(sf::VideoMode(800,600), "Menu", sf::Style::Titlebar | sf::Style::Close);
            sf::Texture logoT;
            logoT.loadFromFile(logoTPath);
            sf::RectangleShape logo;
            const sf::Texture *plogoT = &logoT;
            logo.setSize(sf::Vector2f(logoT.getSize().x, logoT.getSize().y));
            logo.setPosition(VIEW_WIDTH/2.0f, 100);
            logo.setTexture(plogoT);
            logo.setOrigin(logo.getSize()/2.0f);
            bool op1 = true;
            bool op2 = false;
            sf::Text textOp1;
            sf::Text textOp2;
            textOp1.setFont(font);
            textOp2.setFont(font);
            textOp1.setString("Start");
            textOp2.setString("Quit");
            textOp1.setPosition(250, 300);
            textOp2.setPosition(250, 400);
            textOp1.setCharacterSize(100);
            textOp2.setCharacterSize(100);
            while (menu.isOpen())
            {
                sound.playMusic("menu");
                sf::Event evento;
                deltaTime = clock.restart().asSeconds();
                if (deltaTime > 1.0f / 60.f)
                    deltaTime = 1.0f / 60.0f;
                while (menu.pollEvent(evento))
                {
                    //fecha a janela
                    if(evento.type == sf::Event::Closed)
                    {
                        menu.close();
                        gameOpen = false;
                    }

                    if (evento.type== sf::Event::KeyReleased && evento.key.code == sf::Keyboard::Up)
                    {
                        op1 = !op1;
                        op2 = !op2;
                    }
                    else if(evento.type==sf::Event::KeyReleased && evento.key.code == sf::Keyboard::Down)
                    {
                        op1 = !op1;
                        op2 = !op2;
                    }
                    else if(evento.type==sf::Event::KeyReleased && evento.key.code == sf::Keyboard::Return)
                    {
                        //Verifica se vai para o jogo ou sai dele quando aperta enter
                        if(op2)
                        {
                            menu.close();
                            gameOpen = false;
                        }
                        if(op1)
                        {
                            sound.stopMusic("menu");
                            menu.close();
                            inMenu = false;

                        }
                    }

                }
                //Muda a cor do texto de acordo com a opcao selecionada
                if(op1)
                    textOp1.setFillColor(sf::Color::Yellow);
                else
                    textOp1.setFillColor(sf::Color::White);
                if(op2)
                    textOp2.setFillColor(sf::Color::Yellow);
                else
                    textOp2.setFillColor(sf::Color::White);

                menu.clear(sf::Color::Blue);
                menu.draw(logo);
                menu.draw(textOp1);
                menu.draw(textOp2);
                menu.display();

            }
            inMenu = false;
        }
        else
        {
            //Limpa os vetores e coloca o mario na posicao inicial
            coins.clear();
            smushrooms.clear();
            mario.setPosition( sf::Vector2f(5.0f,300.0f));
            mario.setBigMario(false);

            //Instancia os objetos no inicio do jogo
            vidas = 3;
            pontuacao = 0;
            for( int i = 0; i<10; i++)
                coins.push_back(Coin(sf::Vector2f(float(82.0 + 33.0*i), 450.0f), switchTime, coinTexture));
            QuestionBlock quest(sf::Vector2f(100.0,500.0f));

            //Cria a tela do jogo
            game.create(sf::VideoMode(800, 600), "Janela", sf::Style::Titlebar | sf::Style::Close);
            sf::View view(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(800.0f, 600.0f));
            sound.playMusic("game");

            while (game.isOpen())
            {
                sf::Event evento;
                deltaTime = clock.restart().asSeconds();
                if (deltaTime > 1.0f / 20.f)
                    deltaTime = 1.0f / 20.0f;
                while (game.pollEvent(evento))
                {
                    switch (evento.type)
                    {
                    //fecha a window
                    case sf::Event::Closed:
                        game.close();
                        gameOpen = false;
                        break;
                    case sf::Event::KeyReleased:
                        break;
                    case sf::Event::Resized:
                        resizeWindow(game, view);
                        break;
                    }
                }
                //updates
                mario.update(deltaTime, game);
                hud.update(patch::to_string(pontuacao), patch::to_string(vidas), mario.getPosition());
                for( int i =0; i<smushrooms.size(); i++)
                    smushrooms[i].update(deltaTime);
                for( int i =0; i<coins.size(); i++)
                    coins[i].update(deltaTime);
                //Colisoes

                //colisao objetos-plataformas
                for(int i =0; i<platforms.size(); i++)
                {
                    if(mario.getCollider().checkCollision(platforms[i].getCollider(), direction, 0.0f))
                        mario.onCollision(direction);
                    for(int j = 0; j<smushrooms.size(); j++)
                    if(smushrooms[j].getCollider().checkCollision(platforms[i].getCollider(), direction, 0.0f))
                        smushrooms[j].onCollision(direction);
                }
                for( int i =0; i<coins.size(); i++)
                {
                    if(coins[i].getCollider().checkCollision(mario.getCollider(), direction, 0.0f))
                    {
                        coins.erase(coins.begin()+i);
                        sound.playSound("coin");
                        pontuacao++;
                    }
                }
                for(int i = 0; i<smushrooms.size(); i++)
                {
                    if(smushrooms[i].getCollider().checkCollision(mario.getCollider(), direction, 0.0f))
                    {
                        smushrooms[i].onCollisionPlayer(mario);
                        smushrooms.erase(smushrooms.begin()+i);
                    }

                }
                if(mario.getCollider().checkCollision(quest.getCollider(), direction, 0.0f))
                {
                    mario.onCollision(direction);
                    if(quest.getActivate() && quest.onCollision(sf::Vector2f(-1*direction.x, -1*direction.y)))
                        smushrooms.push_back(SuperMushroom(sf::Vector2f(quest.getPosition().x, quest.getPosition().y-32), 50, sMushroomTexture));


                }
                //Controle de queda do cenario
                if(mario.getPosition().y > 600)
                {
                    sound.playSound("death");
                    vidas--;
                    if(vidas>0)
                    {
                        mario.setPosition(sf::Vector2f(5.0f, 300.0f));
                        if(mario.getBigMario())
                        {
                            mario.setBigMario(false);
                            mario.setScale(0.83,0.83);
                        }

                    }
                }
                //Verifica se as vidas acabaram
                if(vidas<=0)
                {
                    inMenu = true;
                    game.close();
                    break;
                }
                //controle de movimento
                /*if(mario.getPosition().x <=0.0)
                    mario.move(sf::Vector2f(1.0f,0.0f));
                */
                //controle da view
                view.setCenter((int)mario.getPosition().x, 300);
                game.setView(view);
                //draw
                game.clear(sf::Color::Blue);
                for( int i =0; i<platforms.size(); i++)
                    platforms[i].draw(game);
                for( int i =0; i<coins.size(); i++)
                    coins[i].draw(game);
                quest.draw(game);
                mario.draw(game);
                for( int i =0; i<smushrooms.size(); i++)
                    smushrooms[i].draw(game);
                hud.draw(game);
                game.display();
            }
        }
    }
    return 0;
}
