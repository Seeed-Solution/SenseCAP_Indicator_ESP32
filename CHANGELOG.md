# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Added

### Changed

### Fixed

## 2024-03-01
### Added
- Grove Vision V2 demo: To display the stream from it

### Changed
- serveral sdkconfig(s) for IDF5.1

### Fixed
- LVGL Highlight turns black
- Dependency library compiling error (Exclude LoRaWAN component for specific example)

## 2024-01-05                

### Added
- Add a terminal png on indicator_lorawan demo

### Changed
- change button title

## 2023-12-18                
### Changed
- Update indicator_lorawan demo README

## 2023-12-15
### Changed
- File structure to dependent
### Fixed
- LoRaWAN frequency change rebooting

## 2023-11-23
### Changed
-  Modify sdkconfig.defaults for lvgl,photo,squareline_demo
### Added
-  Add Project Share to README

## 2023-11-17
### Changed
-  No longer need patching IDF (thx to Hermit from SqureLine Studio)
-  Update README to compile with ESP-IDF v5.1.1

## 2023-10-31
### Added
-  lorawan demo and fix bugs

## 2023-10-09
### Fixed
- Connection bug in Home Assistant (dc0b8b7).

### Added
- Console feature for indicator_ha (87a2536).

## 2023-08-10
### Changed
- Updated main.c in lora_demo (459fb0b).
- Multiple updates to README (5c77285, 24a2ef2).

### Added
- Updates to LoRa Demos (cb2e206).

## 2023-06-20
### Fixed
- Initialization issue in lora_demo (b34f48d).

## 2023-06-07
### Changed
- Updated install_patch.sh (e8335d0).
- Updated README.md (0ff8270).
- Home Assistant configuration updates (3dc1ab5, 5a1a383).

### Added
- Home Assistant action testing and related merge actions (7591f54, 6b299b7, 49cb975).

### Removed
- Reverted "Create HA.yml" (6b299b7).

## 2023-06-05
### Changed
- Updated README.md, set the "Get One Now" Center (fb28050).
- Merged pull request for README update (06c2293).

## 2023-05-30
### Added
- Uploaded install_patch.sh (a19a563).

### Changed
- Updated SenseCAP Indicator wiki link (c362b26).
- Merged pull request for wiki link update (4b1b0c4).

## 2023-05-29
### Fixed
- Wifi state display issue in indicator_openai (bf7ff29).

## 2023-05-23
### Added
- Wifi button event in indicator_ha (698ec9e).

### Fixed
- Display issue for factory reset screen (4819eea).

### Changed
- Enabled CONFIG_SPIRAM_TRY_ALLOCATE_WIFI_LWIP in indicator_ha and indicator_openai (9983946, a7b693f).

## 2023-05-19
### Changed
- Updated README for indicator_ha (82d0295, 73cc6f2).
- Updated wiki URL (9bc469a).
- Renamed to indicator (90189e9).

### Added
- Feature update for home assistant (84a74b3).

## 2023-05-18
### Changed
- Optimized error messages in indicator_openai (17a4296).


## 2023-05-18
### Added
- Scroll dots to improve navigation (3e27a26).
- Home Assistant UI and model handling (0a08978).

### Changed
- Enhanced UI optimization for better user interaction (36496d9).
- OpenAI UI optimization, including color changes and placeholder text modifications (9f91f12).

### Fixed
- Issue with switch triggering when switching screens (9d29a14).
- State saving mechanism of the switch (e681eb7).

## 2023-05-15
### Added
- Indicator_OpenAI demo and updates to README files (60e4aca, 78ee4e7).
- SDK configuration updates for Indicator_OpenAI (e9863cb).

### Fixed
- Function optimization and bug fixes in Indicator_OpenAI (a6c9c14).

## 2023-04-26
### Added
- Button demo for enhanced functionality (b3b59dc).

### Fixed
- Style fixes in the Lora_demo (a8cc33e).

## 2023-04-23
### Added
- Photo demo for visual enhancement (20ce795).

## [1.0.0] 2023-04-20
- Release of the first version of the project. 

## 2023-04-19
### Added
- Initial SDK setup (784bdda).

### Changed
- Creation of the initial README.md for the project (e1f71b8).

## 2023-04-18
### Added
- Initial commit of the project (97479ad).
