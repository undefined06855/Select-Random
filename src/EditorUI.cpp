#include "EditorUI.hpp"
#include "SelectRandomPopup.hpp"
#include <alphalaneous.editortab_api/API.hpp>

bool HookedEditorUI::init(LevelEditorLayer* editor) {
    if (!EditorUI::init(editor)) return false;

    // auto btn = getSpriteButton(
    //     "icon.png"_spr,
    //     menu_selector(HookedEditorUI::onButton),
    //     nullptr,
    //     .9f
    // );
    // btn->setID("editor-button"_spr);

    // m_editButtonBar->m_buttonArray->addObject(btn);

    // int rows = GameManager::sharedState()->getIntGameVariable("0049");
    // int cols = GameManager::sharedState()->getIntGameVariable("0050");
    // m_editButtonBar->reloadItems(rows, cols);

    auto tab = alpha::editor_tabs::getMode(alpha::editor_tabs::Edit)->getTab("edit");
    geode::log::info("{}", (void*)tab);
    tab->getNode()->addItem(
        alpha::editor_tabs::createEditButton(
            cocos2d::CCSprite::createWithSpriteFrameName("icon.png"_spr), /* GJ_button_01.png */ 1,
            [this](CCMenuItemSpriteExtra* button) {
                this->onButton(button);
            }
        )
    );

    return true;
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
