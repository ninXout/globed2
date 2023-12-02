use globed_shared::SpecialUser;

use crate::data::*;

use super::{Color3B, ColorParseError};

#[derive(Clone, Encodable, EncodableWithKnownSize, Decodable)]
pub struct PlayerIconData {
    pub cube: i16,
    pub ship: i16,
    pub ball: i16,
    pub ufo: i16,
    pub wave: i16,
    pub robot: i16,
    pub spider: i16,
    pub swing: i16,
    pub jetpack: i16,
    pub death_effect: i16,
    pub color1: i16,
    pub color2: i16,
}

impl Default for PlayerIconData {
    fn default() -> Self {
        Self {
            cube: 1,
            ship: 1,
            ball: 1,
            ufo: 1,
            wave: 1,
            robot: 1,
            spider: 1,
            swing: 1,
            jetpack: 1,
            death_effect: 1,
            color1: 1,
            color2: 3,
        }
    }
}

impl PlayerIconData {
    pub fn is_valid(&self) -> bool {
        // TODO icon ids validation and stuff.. or not?
        true
    }
}

/* SpecialUserData */

#[derive(Clone, Encodable, EncodableWithKnownSize, Decodable)]
pub struct SpecialUserData {
    pub name_color: Color3B,
}

impl Default for SpecialUserData {
    fn default() -> Self {
        Self {
            name_color: Color3B { r: 255, g: 255, b: 255 },
        }
    }
}

impl TryFrom<SpecialUser> for SpecialUserData {
    type Error = ColorParseError;
    fn try_from(value: SpecialUser) -> Result<Self, Self::Error> {
        Ok(Self {
            name_color: value.color.parse()?,
        })
    }
}

/* PlayerAccountData */

#[derive(Clone, Default, Encodable, EncodableWithKnownSize, Decodable)]
pub struct PlayerAccountData {
    pub account_id: i32,
    pub name: FastString<MAX_NAME_SIZE>,
    pub icons: PlayerIconData,
    pub special_user_data: Option<SpecialUserData>,
}

impl PlayerAccountData {
    pub fn make_preview(&self, level_id: i32) -> PlayerPreviewAccountData {
        PlayerPreviewAccountData {
            account_id: self.account_id,
            name: self.name.clone(),
            cube: self.icons.cube,
            color1: self.icons.color1,
            color2: self.icons.color2,
            level_id,
        }
    }
}

/* PlayerPreviewAccountData - like PlayerAccountData but more limited, for the total player list */

#[derive(Clone, Default, Encodable, EncodableWithKnownSize, Decodable)]
pub struct PlayerPreviewAccountData {
    pub account_id: i32,
    pub name: FastString<MAX_NAME_SIZE>,
    pub cube: i16,
    pub color1: i16,
    pub color2: i16,
    pub level_id: i32,
}

/* PlayerData (data in a level) */

#[derive(Clone, Default, Encodable, EncodableWithKnownSize, Decodable)]
pub struct PlayerData {}

/* AssociatedPlayerData */

#[derive(Clone, Default, Encodable, EncodableWithKnownSize, Decodable)]
pub struct AssociatedPlayerData {
    pub account_id: i32,
    pub data: PlayerData,
}

/* PlayerMetadata (things like your percentage in a level, attempt count) */

#[derive(Clone, Default, Encodable, EncodableWithKnownSize, Decodable)]
pub struct PlayerMetadata {
    percentage: u16,
    attempts: i32,
}

/* AssociatedPlayerMetadata */

#[derive(Clone, Default, Encodable, EncodableWithKnownSize)]
pub struct AssociatedPlayerMetadata {
    pub account_id: i32,
    pub data: PlayerMetadata,
}
