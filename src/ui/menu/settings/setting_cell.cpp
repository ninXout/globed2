#include "setting_cell.hpp"

#include "audio_setup_popup.hpp"
#include "string_input_popup.hpp"
#include <managers/settings.hpp>
#include <util/misc.hpp>

using namespace geode::prelude;
namespace permission = geode::utils::permission;
using permission::Permission;

bool GlobedSettingCell::init(void* settingStorage, Type settingType, const char* nameText, const char* descText, const Limits& limits) {
    if (!CCLayer::init()) return false;
    this->settingStorage = settingStorage;
    this->settingType = settingType;
    this->descText = descText;
    this->limits = limits;

    Build<CCLabelBMFont>::create(nameText, "bigFont.fnt")
        .scale(0.7f)
        .anchorPoint(0.f, 0.5f)
        .pos(10.f, CELL_HEIGHT / 2)
        .parent(this)
        .store(labelName);

    // desc button
    if (descText && descText[0] != '\0') {
        auto labelSize = labelName->getScaledContentSize();
        Build<CCSprite>::createSpriteName("GJ_infoIcon_001.png")
            .scale(0.4f)
            .intoMenuItem([nameText, descText](auto) {
                FLAlertLayer::create(nameText, descText, "Ok")->show();
            })
            .pos(10.f + labelSize.width + 5.f, CELL_HEIGHT / 2 + labelSize.height / 2 - 5.f)
            .intoNewParent(CCMenu::create())
            .pos(0.f, 0.f)
            .parent(this);
    }

    switch (settingType) {
    case Type::Bool: {
        Build<CCMenuItemToggler>(CCMenuItemToggler::createWithStandardSprites(this, menu_selector(GlobedSettingCell::onCheckboxToggled), 1.0f))
            .anchorPoint(1.f, 0.5f)
            .pos(CELL_WIDTH - 10.f, CELL_HEIGHT / 2)
            .scale(0.8f)
            .id("input-checkbox"_spr)
            .store(inpCheckbox)
            .intoNewParent(CCMenu::create())
            .pos(0.f, 0.f)
            .id("input-menu"_spr)
            .parent(this);

        inpCheckbox->toggle(*(bool*)(settingStorage));
    } break;
    case Type::Float: {
        // TODO i give up trying to make this slider smaller
        Build<Slider>::create(this, menu_selector(GlobedSettingCell::onSliderChanged), .5f)
            .anchorPoint(1.f, 0.5f)
            .pos(CELL_WIDTH - 60.f, CELL_HEIGHT / 2)
            .parent(this)
            .id("input-slider"_spr)
            .store(inpSlider);

        float value = *(float*)(settingStorage);
        float relativeValue = value / (limits.floatMax - limits.floatMin);
        inpSlider->setValue(relativeValue);
    } break;
    case Type::String: [[fallthrough]];
    case Type::AudioDevice: {
        Build<CCMenuItemSpriteExtra>::create(
            Build<ButtonSprite>::create("Set", "goldFont.fnt", "GJ_button_04.png", .8f).collect(),
            this,
            menu_selector(GlobedSettingCell::onInteractiveButton)
        )
            .anchorPoint(0.5f, 0.5f)
            .pos(CELL_WIDTH - 10.f, CELL_HEIGHT / 2)
            .id("input-interactive-btn"_spr)
            .store(inpAudioButton)
            .intoNewParent(CCMenu::create())
            .pos(0.f, 0.f)
            .id("input-menu"_spr)
            .parent(this);

        inpAudioButton->setPositionX(inpAudioButton->getPositionX() - inpAudioButton->getScaledContentSize().width / 2);
    } break;
    case Type::Int: {
        int currentValue = *(int*)(settingStorage);
        Build<InputNode>::create(CELL_WIDTH * 0.2f, "", "chatFont.fnt", std::string(util::misc::STRING_DIGITS), 10)
            .anchorPoint(1.f, 0.5f)
            .pos(CELL_WIDTH - 10.f, CELL_HEIGHT / 2)
            .id("input-field"_spr)
            .store(inpField)
            .intoNewParent(CCMenu::create())
            .pos(0.f, 0.f)
            .id("input-menu"_spr)
            .parent(this);

        inpField->setString(std::to_string(currentValue));
    } break;
    }

    return true;
}

void GlobedSettingCell::onCheckboxToggled(cocos2d::CCObject*) {
    this->storeAndSave(!inpCheckbox->isOn());
}

void GlobedSettingCell::onSliderChanged(cocos2d::CCObject*) {
    float relativeValue = inpSlider->getThumb()->getValue();
    float value = limits.floatMin + (limits.floatMax - limits.floatMin) * relativeValue;
    this->storeAndSave(value);
}

void GlobedSettingCell::onInteractiveButton(cocos2d::CCObject*) {
    if (settingType == Type::AudioDevice) {
#if GLOBED_VOICE_SUPPORT
        // check for permission
        bool perm = permission::getPermissionStatus(Permission::RecordAudio);

        if (!perm) {
            geode::createQuickPopup(
                "No permission",
                "Globed does not currently have permission to use your microphone. Do you want to grant the permission?",
                "Cancel", "Grant", [] (FLAlertLayer*, bool btn2) {

                if (btn2) {
                    permission::requestPermission(Permission::RecordAudio, [](bool granted) {
                        if (granted) {
                            AudioSetupPopup::create()->show();
                        } else {
                            log::warn("permission denied when requesting audio access");
                        }
                    });
                }
            });

            return;
        }
        AudioSetupPopup::create()->show();
#endif // GLOBED_VOICE_SUPPORT
    } else {
        StringInputPopup::create([this](const std::string_view text) {
            this->onStringChanged(text);
        })->show();
    }
}

void GlobedSettingCell::onStringChanged(const std::string_view text) {
    this->storeAndSave(text);
}

void GlobedSettingCell::storeAndSave(std::any value) {
    // banger
    switch (settingType) {
    case Type::Bool:
        *(bool*)(settingStorage) = std::any_cast<bool>(value); break;
    case Type::Float:
        *(float*)(settingStorage) = std::any_cast<float>(value); break;
    case Type::String:
        *(std::string*)(settingStorage) = std::any_cast<std::string>(value); break;
    case Type::AudioDevice: [[fallthrough]];
    case Type::Int:
        *(int*)(settingStorage) = std::any_cast<int>(value); break;
    }

    GlobedSettings::get().save();
}

GlobedSettingCell* GlobedSettingCell::create(void* settingStorage, Type settingType, const char* name, const char* desc, const Limits& limits) {
    auto ret = new GlobedSettingCell;
    if (ret->init(settingStorage, settingType, name, desc, limits)) {
        return ret;
    }

    delete ret;
    return nullptr;
}