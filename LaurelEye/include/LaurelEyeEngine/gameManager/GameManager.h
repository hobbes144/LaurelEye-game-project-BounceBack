#pragma once

namespace LaurelEye {

    class GameManager {
    public:
        // --- Generators Destroyed ---
        void addGeneratorDestroyed(float value = 1.0f) { gensDestroyed += static_cast<int>(value); }
        float getGeneratorsDestroyed() const { return static_cast<float>(gensDestroyed); }

        // --- Player Health ---
        void setPlayerHealth(float value) { playerHealth = value; }
        float getPlayerHealth() const { return playerHealth; }

        // --- Time Completed ---
        void addTime(float value) { timeCompleted += value; }
        void setTimeCompleted(float value) { timeCompleted = value; }
        float getTimeCompleted() const { return timeCompleted; }

        // --- Enemies Killed ---
        void addEnemyKilled(float value = 1.0f) { enemiesKilled += static_cast<int>(value); }
        float getEnemiesKilled() const { return static_cast<float>(enemiesKilled); }

        // --- Combos ---
        void addCombo(float value = 1) { combos += static_cast<int>(value); }
        void resetCombo() { combos = 0; }
        float getCombos() const { return static_cast<float>(combos); }

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
        float playerHealth = 3.0f;
        float timeCompleted = 0.0f;
        int enemiesKilled = 0;
        int combos = 0;
    };

} // namespace LaurelEye
