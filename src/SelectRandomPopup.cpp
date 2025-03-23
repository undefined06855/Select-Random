#include "SelectRandomPopup.hpp"

SelectRandomPopup* SelectRandomPopup::create() {
    auto ret = new SelectRandomPopup;
    if (ret->initAnchored(240.f, 230.f)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool SelectRandomPopup::setup() {
    setTitle("Select Random");
    setID("select-random-popup"_spr);

    m_percentage = 0.f;

    m_chanceInput = geode::TextInput::create(70.f, "50", "bigFont.fnt");
    m_chanceInput->setID("percentage-input");
    m_chanceInput->setFilter("0123456789");
    m_chanceInput->setString(geode::Mod::get()->getSavedValue<std::string>("last-percentage", "50"), true);
    m_chanceInput->setCallback([this](const std::string& str){ onChanceInput(str); });
    m_mainLayer->addChildAtPosition(m_chanceInput, geode::Anchor::Center, { 0.f, 54.f });

    m_infoLabel = cocos2d::CCLabelBMFont::create("...", "chatFont.fnt");
    m_infoLabel->setID("info-label");
    m_mainLayer->addChildAtPosition(m_infoLabel, geode::Anchor::Center, { 0.f, 25.f });

    auto percentageLabel = cocos2d::CCLabelBMFont::create("%", "bigFont.fnt");
    percentageLabel->setID("percentage-label");
    percentageLabel->setScale(.6f);
    m_mainLayer->addChildAtPosition(percentageLabel, geode::Anchor::Center, { 47.f, 54.f });

    auto buttonSprite = ButtonSprite::create("Select!");
    buttonSprite->setScale(.9f);
    buttonSprite->setCascadeOpacityEnabled(true);
    m_selectButton = CCMenuItemSpriteExtra::create(
        buttonSprite, this, menu_selector(SelectRandomPopup::onButton)
    );
    m_selectButton->setID("select-btn");
    m_selectButton->setCascadeOpacityEnabled(true);
    m_buttonMenu->addChildAtPosition(m_selectButton, geode::Anchor::Bottom, { 0.f, 25.f });

    auto checkboxMenu = cocos2d::CCMenu::create();
    checkboxMenu->setID("checkbox-menu");
    checkboxMenu->setLayout(
        geode::RowLayout::create()
            ->setGap(4.5f)
            ->setAutoScale(false)
    );
    checkboxMenu->setContentSize({ 200.f, 25.f });
    m_mainLayer->addChildAtPosition(checkboxMenu, geode::Anchor::Center, { 0.f, -8.f });

        m_checkbox = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(SelectRandomPopup::onCheckbox), .75f);
        m_checkbox->setID("checkbox");
        // have to call four times to be consistent, ccmenuitemtoggler actually sucks major balls
        // there is CERTAINLY a better way to do this
        if (geode::Mod::get()->getSavedValue<bool>("last-checkbox", true)) m_checkbox->toggle(true);
        checkboxMenu->addChild(m_checkbox);

        auto checkboxLabel = cocos2d::CCLabelBMFont::create("Treat linked objects as one", "bigFont.fnt");
        checkboxLabel->setID("checkbox-label");
        checkboxLabel->setScale(.35f);
        checkboxMenu->addChild(checkboxLabel);

    checkboxMenu->updateLayout();

    auto seedLabel = cocos2d::CCLabelBMFont::create("Seed:", "bigFont.fnt");
    seedLabel->setID("seed-label");
    seedLabel->setScale(.55f);
    m_mainLayer->addChildAtPosition(seedLabel, geode::Anchor::Center, { 0.f, -37.f });

    m_seedInput = geode::TextInput::create(130.f, "(random)", "bigFont.fnt");
    m_seedInput->setID("seed-input");
    m_seedInput->setFilter("0123456789");
    m_seedInput->setString(geode::Mod::get()->getSavedValue<std::string>("last-seed", ""), true);
    m_seedInput->setCallback([this](const std::string& str){ onSeedInput(str); });
    m_seedInput->setMaxCharCount(5);
    m_seedInput->setScale(.775f);
    m_mainLayer->addChildAtPosition(m_seedInput, geode::Anchor::Center, { 0.f, -58.f });

    // update stuffs like members and labels
    onSeedInput(m_seedInput->getString()); // sets m_seed
    onCheckbox(nullptr);

    return true;
}

void SelectRandomPopup::onChanceInput(std::string str) {
    if (str == "") {
        m_percentage = .5f;
        return;
    }

    float asFloat = std::atof(str.c_str());
    if (asFloat == 0) {
        // invalid perhaps
        m_percentage = 0.f;
        m_chanceInput->setString("0");
        str = "0";
    } else {
        if (asFloat > 100) {
            asFloat = 100;
            m_chanceInput->setString("100");
            str = "100";
        }

        m_percentage = asFloat / 100.f;
    }

    geode::Mod::get()->setSavedValue<std::string>("last-percentage", str);

    int percentage = m_percentage * 100.f;
    int total = actualSelectedObjects()->count();
    int result = total * m_percentage;
    m_infoLabel->setString(fmt::format("{}% x {} objects -> {} selected objects", percentage, total, result).c_str());
    m_infoLabel->limitLabelWidth(180.f, 0.85f, 0.2f);

    if (result == 0) {
        m_selectButton->setEnabled(false);
        m_selectButton->setOpacity(127);
    } else {
        m_selectButton->setEnabled(true);
        m_selectButton->setOpacity(255);
    }
}

void SelectRandomPopup::onSeedInput(std::string str) {
    if (str == "") {
        m_seed = 0;
        return;
    }

    if (str.length() > 5) {
        // stupid textbox length bypass people
        str = str.substr(0, 5);
    }

    geode::Mod::get()->setSavedValue<std::string>("last-seed", str);
    m_seed = std::stoi(str);
}

void SelectRandomPopup::onCheckbox(cocos2d::CCObject* sender) {
    if (sender) m_treatLinkedObjectsAsOne = !m_checkbox->isOn(); // being called from callback, reverse
    else m_treatLinkedObjectsAsOne = m_checkbox->isOn(); // being called from init
    geode::Mod::get()->setSavedValue<bool>("last-checkbox", m_treatLinkedObjectsAsOne);
    onChanceInput(m_chanceInput->getString()); // update label n shit
}

cocos2d::CCArray* SelectRandomPopup::actualSelectedObjects() {
    if (!m_treatLinkedObjectsAsOne) {
        return EditorUI::get()->getSelectedObjects()->shallowCopy();
    }

    // for every collection of linked objects, only return one object from each group

    auto objs = EditorUI::get()->getSelectedObjects();
    std::vector<int> linkedGroupsSeen = {};
    cocos2d::CCArray* results = cocos2d::CCArray::create();

    for (auto obj : geode::cocos::CCArrayExt<GameObject*>(objs)) {
        if (obj->m_linkedGroup == -1) {
            // not a linked object
            results->addObject(obj);
            continue;
        }

        if (std::find(linkedGroupsSeen.begin(), linkedGroupsSeen.end(), obj->m_linkedGroup) != linkedGroupsSeen.end()) {
            // already seen
            continue;
        }

        linkedGroupsSeen.push_back(obj->m_linkedGroup);
        results->addObject(obj);
    }

    return results;
}

void SelectRandomPopup::onButton(cocos2d::CCObject* sender) {
    auto editorUI = EditorUI::get();
    auto objects = actualSelectedObjects();

    if (std::floor(objects->count() * m_percentage) == 0) {
        // button should really be disabled but idk just in case??
        geode::Notification::create("No objects will be selected!", geode::NotificationIcon::Info);
        return;
    }

    onClose(sender);

    editorUI->deselectAll();
    // undo objects are too odd
    // LevelEditorLayer::get()->m_undoObjects->removeLastObject();

    auto results = cocos2d::CCArray::create();

    if (m_seed == 0) std::srand(std::time(NULL));
    else             std::srand(m_seed);

    int total = m_percentage * objects->count();
    for (int i = 0; i < total; i++) {
        auto p = ((float)std::rand() / (float)RAND_MAX);
        int indexToRemove = p * objects->count();
        auto obj = objects->objectAtIndex(indexToRemove);
        results->addObject(obj);
        objects->fastRemoveObjectAtIndex(indexToRemove);
    }

    editorUI->selectObjects(results, true);
    editorUI->updateEditMenu();
    editorUI->updateButtons();
    editorUI->updateObjectInfoLabel();
}

// TODO: treat linked objects as one
