#include <Geode/Geode.hpp>
#include <Geode/modify/LevelTools.hpp>
#include <Geode/modify/LevelSelectLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
using namespace geode::prelude;

constexpr int ID_EXPLORERS = 23;
constexpr int ID_FIREBIRD = 24;

// Estrellas que se MUESTRAN (no se entregan al completar)
static int starsFor(int id) {
    if (id == ID_EXPLORERS) return 17;
    if (id == ID_FIREBIRD) return 20;
    return 0;
}

static bool isCustom(int id) {
    return id == ID_EXPLORERS || id == ID_FIREBIRD;
}

// Nivel de Fire Bird (de prueba). Para poner uno real, reemplaza este texto.
static const char* FIREBIRD_STRING = "H4sIADtZ3moC_6WTy23EMAwFG1IA_iUjp61hC2AB20KKj9fS2uCDDwFyET1-IgENwNdTR-M0SkkWT01xT-ZZZJb50_KLM5KJKHtysr-PkZQj-YfzGEHytxH8_xHbzYj2erA2ehefJWaxtp_zu88_q4x3eep2kBznHHAEDzvOmTLNwo2-uY0mLYiaNvYPasWouJ3IO25Rsaa7BWAvw-Z1--CKL1Yp11lHHRe3fPV3rv3dgHthIQOG3AhYgQO46pAewJAPAa661v3zfSs_Wbn6Uq5-1G756vfqS92Aqw8dkI-amxCwAgdw9WEewJCHAFdf6_75vpVfvFVftlU_Lrd89rtWX64GXH14QB413xcRGFaRYBep-ggNYMhNgB02v_pa-cW9-or-8fML-9KzVoEFAAA=";

class $modify(LevelTools) {
    static GJGameLevel* getLevel(int id, bool loadString) {
        if (!isCustom(id)) return LevelTools::getLevel(id, loadString);

        auto lvl = GJGameLevel::create();
        lvl->m_levelID = id;
        lvl->m_levelName = (id == ID_EXPLORERS) ? "Explorers" : "Fire Bird";
        lvl->m_levelType = static_cast<GJLevelType>(1);

        // Se MUESTRA como demon con estrellas
        lvl->m_stars = starsFor(id);
        lvl->m_demon = true;
        lvl->m_difficulty = GJDifficulty::Demon;
        lvl->m_demonDifficulty = 0;

        if (loadString) {
            if (id == ID_EXPLORERS) {
                auto path = Mod::get()->getResourcesDir() / "nivel.txt";
                auto res = utils::file::readString(path);
                if (res) lvl->m_levelString = res.unwrap();
            } else {
                lvl->m_levelString = FIREBIRD_STRING;
            }
        }
        return lvl;
    }
};

// Al completar, estrellas en 0 un instante para que NO den recompensas.
class $modify(PlayLayer) {
    void levelComplete() {
        if (m_level && isCustom(m_level->m_levelID)) {
            int real = starsFor(m_level->m_levelID);
            m_level->m_stars = 0;
            PlayLayer::levelComplete();
            m_level->m_stars = real;
            return;
        }
        PlayLayer::levelComplete();
    }
};

// Mete Explorers y Fire Bird en el selector, despues de Dash.
class $modify(LevelSelectLayer) {
    bool init(int page) {
        if (!LevelSelectLayer::init(page)) return false;

        auto lvl1 = LevelTools::getLevel(ID_EXPLORERS, true);
        auto lvl2 = LevelTools::getLevel(ID_FIREBIRD, true);
        if (lvl1 && lvl2 && m_scrollLayer) {
            lvl1->retain();
            lvl2->retain();
            auto sl = m_scrollLayer;
            if (sl->m_dynamic && sl->m_dynamicObjects) {
                int idx = std::min<int>(22, sl->m_dynamicObjects->count());
                sl->m_dynamicObjects->insertObject(lvl1, idx);
                sl->m_dynamicObjects->insertObject(lvl2, idx + 1);
                sl->updatePages();
            }
        }
        Notification::create("Explorers hook OK", NotificationIcon::Success)->show();
        return true;
    }
};
