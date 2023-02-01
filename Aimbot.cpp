
#pragma once
#include <vector>
#include "LocalPlayer.cpp"
#include "Player.cpp"
#include "Math.cpp"
#include "Level.cpp"
#include "math.h"
#include "X11Utils.cpp"
#include "ConfigLoader.cpp"

class Aimbot
{
private:
    ConfigLoader *m_configLoader;
    Level *m_level;
    LocalPlayer *m_localPlayer;
    std::vector<Player *> *m_players;
    X11Utils *m_x11Utils;

    Player *m_lockedOnPlayer = nullptr;

public:
    Aimbot(ConfigLoader *configLoader,
           Level *level,
           LocalPlayer *localPlayer,
           std::vector<Player *> *players,
           X11Utils *x11Utils)
    {
        m_configLoader = configLoader;
        m_level = level;
        m_localPlayer = localPlayer;
        m_players = players;
        m_x11Utils = x11Utils;
    }
    void update()
    {
        // validations
        if (m_configLoader->getAimbotTrigger() != 0x0000)
        { // our trigger is a button
            if (!m_x11Utils->keyDown(m_configLoader->getAimbotTrigger()))
            {
                m_lockedOnPlayer = nullptr;
                return;
            }
        }
        if (!m_level->isPlayable())
        {
            m_lockedOnPlayer = nullptr;
            return;
        }
        if (m_localPlayer->isDead())
        {
            m_lockedOnPlayer = nullptr;
            return;
        }
        if (m_localPlayer->isKnocked())
        {
            m_lockedOnPlayer = nullptr;
            return;
        }
        if (m_configLoader->getAimbotTrigger() == 0x0000) // our trigger is localplayer attacking
            if (!m_localPlayer->isInAttack())
            {
                m_lockedOnPlayer = nullptr;
                return;
            }

        // get desired angle to an enemy
        double desiredViewAngleYaw = 0;
        double desiredViewAnglePitch = 0;
        if (m_level->isTrainingArea())
        {
     desiredViewAngleYaw = calculateDesiredYaw(localPlayer->getLocationX(),
                                                      localPlayer->getLocationY(),
                                                      31518,
                                                      -6712);
        }
        else
        {
            Player *closestEnemy = findClosestEnemy(localPlayer, players);
            if (closestEnemy == nullptr)
                return;
            desiredViewAngleYaw = calculateDesiredYaw(localPlayer->getLocationX(),
                                                      localPlayer->getLocationY(),
                                                      closestEnemy->getLocationX(),
                                                      closestEnemy->getLocationY());
        }
        const double yaw = localPlayer->getYaw();
        const double angleDelta = calculateAngleDelta(yaw, desiredViewAngleYaw);
        const double angleDeltaAbs = abs(angleDelta);
        if (angleDeltaAbs > m_activationFOV)
            return;
        double newYaw = flipYawIfNeeded(yaw + (angleDelta / m_smoothing));
        localPlayer->setYaw(newYaw);
    }
     double flipYawIfNeeded(double angle)
    {
        double myAngle = angle;
        if (myAngle > 180)
            myAngle = (360 - myAngle) * -1 + (double)rand()/(RAND_MAX)+(rand()%4);
        else if (myAngle < -180)
            myAngle = (360 + myAngle);
        return myAngle;
    }
    double calculatePitchAngleDelta(double oldAngle, double newAngle)
    {
        double wayA = newAngle - oldAngle;
        return wayA;
    }
    double calculateAngleDelta(double oldAngle, double newAngle)
    {
        double wayA = newAngle - oldAngle;
        double wayB = 360 - abs(wayA);
        if (wayA > 0 && wayB > 0)
            wayB *= -1 + (double)rand()/(RAND_MAX)+(rand()%4);
        if (abs(wayA) < abs(wayB))
            return wayA;
        return wayB;
      }
    double calculateDesiredYaw(
        double localPlayerLocationX,
        double localPlayerLocationY,
        double enemyPlayerLocationX,
        double enemyPlayerLocationY)
    {
        const double locationDeltaX = enemyPlayerLocationX - localPlayerLocationX;
        const double locationDeltaY = enemyPlayerLocationY - localPlayerLocationY;
        const double yawInRadians = atan2(locationDeltaY, locationDeltaX);
        const double yawInDegrees = yawInRadians * (180 / M_PI);
        return yawInDegrees;
    }
    double calculateDesiredPitch(
        double localPlayerLocationX,
        double localPlayerLocationY,
        double localPlayerLocationZ,
        double enemyPlayerLocationX,
        double enemyPlayerLocationY,
        double enemyPlayerLocationZ)
    {
        const double locationDeltaZ = enemyPlayerLocationZ - localPlayerLocationZ;
        const double distanceBetweenPlayers = math::calculateDistance2D(enemyPlayerLocationX, enemyPlayerLocationY, localPlayerLocationX, localPlayerLocationY);
        const double pitchInRadians = atan2(-locationDeltaZ, distanceBetweenPlayers);
        const double pitchInDegrees = pitchInRadians * (180 / M_PI);
        return pitchInDegrees;
    }
    Player *findClosestEnemy(LocalPlayer *localPlayer, std::vector<Player *> *players)
    {
        Player *closestPlayerSoFar = nullptr;
        double closestPlayerAngleSoFar;
        for (int i = 0; i < players->size(); i++)
        {
            Player *player = players->at(i);
            if (!player->isValid())
                continue;
            if (player->isKnocked())
                continue;
            if (player->getTeamNumber() == localPlayer->getTeamNumber())
                continue;
            if (!player->isVisible())
                continue;
            double desiredViewAngleYaw = calculateDesiredYaw(localPlayer->getLocationX(),
                                                             localPlayer->getLocationY(),
                                                             player->getLocationX(),
                                                             player->getLocationY());
            double angleDelta = calculateAngleDelta(localPlayer->getYaw(), desiredViewAngleYaw);
            if (closestPlayerSoFar == nullptr)
            {
                closestPlayerSoFar = player;
                closestPlayerAngleSoFar = abs(angleDelta);
            }
            else
            {

                if (abs(angleDelta) < closestPlayerAngleSoFar)
                {
                    closestPlayerSoFar = player;
                    closestPlayerAngleSoFar = abs(angleDelta);
                }
            }
        }
        return closestPlayerSoFar;
    }
};
