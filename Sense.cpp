#pragma once
#include <vector>
#include "LocalPlayer.cpp"
#include "Player.cpp"
#include "Math.cpp"
#include "Level.cpp"
#include "X11Utils.cpp"

class Sense
{
public:
    void update(Level *level, LocalPlayer *localPlayer, std::vector<Player *> *players, X11Utils *x11Utils)
    {
        if (!level->isPlayable())
            return;
        for (int i = 0; i < players->size(); i++)
        {
            Player *player = players->at(i);
            if (!player->isValid())
                continue;
            if (player->getTeamNumber() == localPlayer->getTeamNumber())
                continue;
            if (player->isVisible())
          
                player->setCustomGlow();
                player->setCustomGlowWhite();

                if(player->getShieldValue() <= 10){
                    player->setCustomGlowGreen();
                } else

                if(player->getShieldValue() <= 50){
                    player->setCustomGlowWhite();
                } else

                if(player->getShieldValue() <= 75){
                    player->setCustomGlowBlue();
                } else

                if(player->getShieldValue() <= 100){
                    player->setCustomGlowPurple();
                } else

                if(player->getShieldValue() <= 125){
                    player->setCustomGlowRed();
                }


             }
              else
            {
                    Player->setGlowEnable(7);
                Player->setGlowThroughWall(2);
            }
                Player->setCustomGlow();
                Player->setCustomGlowRed();
            } 
        };
