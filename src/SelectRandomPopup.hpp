#pragma once
#include <Geode/ui/Popup.hpp>

class SelectRandomPopup : public geode::Popup<> {
public:
    static SelectRandomPopup* create();
    bool setup();
    
    cocos2d::CCLabelBMFont* m_infoLabel;

    CCMenuItemToggler* m_checkbox;
    CCMenuItemToggler* m_percentageToggler;
    geode::TextInput* m_chanceInput;
    geode::TextInput* m_seedInput;

    float m_percentage;
    bool m_treatLinkedObjectsAsOne;
    bool m_treatPercentageAsExactValue;
    int m_seed;

    CCMenuItemSpriteExtra* m_selectButton;

    void onButton(cocos2d::CCObject* selector);
    void onChanceInput(std::string str);
    void onSeedInput(std::string str);
    void onCheckbox(cocos2d::CCObject* selector);
    void onPercentage(cocos2d::CCObject* selector);

    cocos2d::CCArray* actualSelectedObjects();
};
