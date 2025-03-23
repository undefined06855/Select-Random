#pragma once
#include <Geode/modify/EditorUI.hpp>

class $modify(HookedEditorUI, EditorUI) {
	void createMoveMenu();
    void onButton(CCObject* sender);
};
