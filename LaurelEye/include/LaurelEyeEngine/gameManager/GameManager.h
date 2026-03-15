#pragma once

namespace LaurelEye {

    class GameManager {
    public:
        // --- Generators Destroyed ---
        void addGeneratorDestroyed(int value = 1) { gensDestroyed += value; }
        int getGeneratorsDestroyed() const { return gensDestroyed; }

        // --- Player Health ---
        void setPlayerHealth(int value) { playerHealth = value; }
        int getPlayerHealth() const { return playerHealth; }

        // --- Time Completed ---
        void addTime(float value) { timeCompleted += value; }
        void setTimeCompleted(float value) { timeCompleted = value; }
        float getTimeCompleted() const { return timeCompleted; }

        // --- Enemies Killed ---
        void addEnemyKilled(int value = 1) { enemiesKilled += value; }
        int getEnemiesKilled() const { return enemiesKilled; }

        // --- Combos ---
        void addCombo(int value = 1) { combos += value; }
        void resetCombo() { combos = 0; }
        int getCombos() const { return combos; }

        // --- Reset All Stats ---
        void resetStats() {
            gensDestroyed = 0;
            playerHealth = 3;
            timeCompleted = 0.0f;
            enemiesKilled = 0;
            combos = 0;
        }

    private:
        int gensDestroyed = 0;
        int playerHealth = 3;
        float timeCompleted = 0.0f;
        int enemiesKilled = 0;
        int combos = 0;
    };

} // namespace LaurelEye
