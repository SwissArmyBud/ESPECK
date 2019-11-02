# ESPECK Interface Kit

## Overview
The ESPECK ADK uses ESPALIF as the interface creation and file bundling system. The interface files are written in "vanilla" HTML/JS/CSS and compiled into single page applications. Each folder inside `Interface/especk/src` is turned into an SPA and added to `Firmware/data` along with the contents of `Interface/especk/static`, when the project is built. More information can be found at the [ESPALIF repo](https://github.com/SwissArmyBud/ESPALIF) - the following functions are enabled by the system:

| command | action |
| :--- | :--- |
| `npm run build` | builds a distributable set of files in `Firmware/data` |
| `npm run watch` | like `build` but watches `especk/src` and rebuilds on changes |
| `npm run serve` | serves the build locally for UI checking and logic debug |

## Architecture
ESPALIF builds a set of SPAs by minimizing content, performing injections into BAMR templates, and then minimizing that output into the `dist` folder, where it also copies any files from the `static` directory. This set of files can then be copied into any static delivery system to provide the GUIs to clients.

**Visual Concept:**
<p align="center">
    <img
      alt="Architecture"
      src="/assets/espalif_arch.png"
    />
</p>
