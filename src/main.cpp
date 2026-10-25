#include <Geode/Geode.hpp>
#include <Geode/modify/LevelTools.hpp>
#include <Geode/modify/LevelSelectLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
using namespace geode::prelude;

constexpr int NEW_ID = 23;
constexpr int FAKE_STARS = 17;

class $modify(LevelTools) {
    static GJGameLevel* getLevel(int id, bool loadString) {
        if (id != NEW_ID) return LevelTools::getLevel(id, loadString);

        auto lvl = GJGameLevel::create();
        lvl->m_levelID = NEW_ID;
        lvl->m_levelName = "Explorers";
        lvl->m_levelType = GJLevelType::Local;

        // Se MUESTRA como demon de 17 estrellas
        lvl->m_stars = FAKE_STARS;
        lvl->m_demon = true;
        lvl->m_difficulty = GJDifficulty::Demon;
        lvl->m_demonDifficulty = 0; // 0 = hard demon (ajustar si queres otro)

        if (loadString) {
            auto path = Mod::get()->getResourcesDir() / "nivel.txt";
            auto res = utils::file::readString(path);
            if (res) lvl->m_levelString = res.unwrap();
        }
        return lvl;
    }
};

// Al completar, ponemos las estrellas en 0 un instante para que NO
// se den estrellas, orbes ni diamantes, y las restauramos despues.
class $modify(PlayLayer) {
    void levelComplete() {
        if (m_level && m_level->m_levelID == NEW_ID) {
            m_level->m_stars = 0;
            PlayLayer::levelComplete();
            m_level->m_stars = FAKE_STARS;
            return;
        }
        PlayLayer::levelComplete();
    }
};

class $modify(LevelSelectLayer) {
    bool init(int page) {
        if (!LevelSelectLayer::init(page)) return false;
        // TODO: agregar la pagina 23 al m_scrollLayer (copiar estilo de Dash)
        return true;
    }
};
