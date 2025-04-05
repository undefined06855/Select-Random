#include "EditorUI.hpp"
#include "SelectRandomPopup.hpp"

void HookedEditorUI::createMoveMenu() {
    EditorUI::createMoveMenu();

    auto btn = getSpriteButton(
        "icon.png"_spr,
        menu_selector(HookedEditorUI::onButton),
        nullptr,
        .9f
    );
    btn->setID("editor-button"_spr);

    m_editButtonBar->m_buttonArray->addObject(btn);

    int rows = GameManager::sharedState()->getIntGameVariable("0049");
    int cols = GameManager::sharedState()->getIntGameVariable("0050");
    m_editButtonBar->reloadItems(rows, cols);
}

void HookedEditorUI::onButton(cocos2d::CCObject* sender) {
    if (m_selectedObjects->count() == 0) {
        geode::Notification::create("Select some objects first!", geode::NotificationIcon::Info)->show();
        return;
    }

    if (m_selectedObjects->count() == 1) {
        geode::Notification::create("Select more than one object first!", geode::NotificationIcon::Info)->show();
        return;
    }

    SelectRandomPopup::create()->show();
}
