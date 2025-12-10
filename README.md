<h1 align="center"> ReLib </h1>
<p align="center">
  Qt program to search/read my manga library <br>
  Rewrite of <a href="https://github.com/nisemono0/HLib">HLib</a> with added multithreading, tag search and autocompletion <br>
</p>

## Info
Program only works with `*.zip` and `*.cbz` archives that contain an `info.json` file inside them. <br>
The `info.json` file stores the respective archive's tags (artist, title, etc.). <br>
Archives <ins>**MUST**</ins> contain an `info.json`, otherwise they get ignored (For now, this might change in the future). <br>
Internally the program uses the archive's basename for item display. <br>

## Format of `info.json`
```json
{
    "gallery_info": {
        "title": "Title [Single] (Uses file basename if Title doesn't exist",
        "tags": {
            "artist": [
                "Artist [Multiple]"
            ],
            "parody": [
                "Parody [Multiple]"
            ],
            "circle": [
                "Circle [Multiple]"
            ],
            "magazine": [
                "Magazine [Multiple]"
            ],
            "event": [
                "Event [Multiple]"
            ],
            "publisher": [
                "Publisher [Multiple]"
            ],
            "other": [
                "Other tags [Multiple]"
            ]
        }
    }
}
```

## Search
Search can be done normally or using a tag namespace. <br>
When searching normally the item display name is used for matching. <br>
Tag namespace search has the following format:
- `namespace:{term_1, term_2}`
  - This returns entries that contain `term_1` <ins>**OR**</ins> `term_2`.
- `namespace:{term_1} namespace:{term_2}`
  - This returns entries that contain `term_1` <ins>**AND**</ins> `term_2`.

Available namespaces: `file_hash` `title` `artist` `parody` `circle` `magazine` `event` `publisher` `tag`. <br>
  - When using the `file_hash` namespace, the rest are ignored.

Autocompletion is triggered when inside a namespace search. <br>
  - For example when typing `artist:{arti|}` a popup will appear with artist tags containing the word `arti`.
  - Works for all namespaces.
  - Can be slow-ish sometimes when a **LOT** of tags are present.

## Shortcuts
### File
Keybind | Description
|:-:|:-:|
`Ctrl+A F` | Open add file to database dialog
`Ctrl+A D` | Open add directory to database dialog
`Ctrl+Q` | Quit

### Database
Keybind | Description
|:-:|:-:|
`Ctrl+D C` | Open create database dialog
`Ctrl+D L` | Open load database dialog
`Ctrl+D U` | Unload the database
`Ctrl+D C H` | Start a databse file hash check
`Ctrl+D C P` | Start a database file path check
`Ctrl+D R E` | Remove entries from the database not found on disk

### Settings
Keybind | Description
|:-:|:-:|
`Ctrl+S I S` | Enable/Disable image scaling
`Ctrl+=` | Increase scale value
`Ctrl+-` | Decrease scale value
`Ctrl+S I F` | Fit image in view
`Ctrl+S I W` | Fit image to width
`Ctrl+S S A` | Enable/Disable search autocomplete
`Ctrl+S S T` | Enable/Disable automatic searching while typing
`Ctrl+S S F` | Enable/Disable selecting first item after a search

### Info
Keybind | Descriptio
|:-:|:-:|
`Ctrl+I L` | Show log window

### Library view
Keybind | Description
|:-:|:-:|
`Down`, `j` | Select next library item
`Up`, `k` | Select previous library item
`gg` | Select first library item
`Shift+G` | Select last library item
`Ctrl+R` | Select random library item

### Image view
Keybind | Description
|:-:|:-:|
`Right`, `l` | Go to next image
`Left`, `h` | Go to previous image
`o` | Load all images of current library item
`gi` | Open jump to image menu (only works if all images are loaded)

### Search
Keybind | Description
|:-:|:-:|
`Ctrl+C` | Refresh search
`Ctrl+F`, `/` | Focus the search input
`Esc` | Unfocus the search input if focused
`Ctrl+L` | Move text cursor once right
`Ctrl+H` | Move text cursor once left
`Alt+L` | Move text cursor one word right
`ALt+H` | Move text cursor once word left
`Enter` | Search

### Completer
Keybind | Description
|:-:|:-:|
`Enter` | Insert selected completion
`Tab`, `Ctrl+J` | Select next completion
`Shift+Tab`, `Ctrl+k` | Select previous completion

## Dependencies (Arch Linux)
`qt6-base` <br>
`quazip-qt6` <br>
`cmake (compile)` <br>

## Compile (Arch Linux)
Install the dependencies then run: <br>
`cmake -B build -DCMAKE_BUILD_TYPE=Release` <br>
`cmake --build build` <br>
The binary is `./build/ReLib`. <br>

## Compile (Windows)
In theory compiling should work on windows too. <br>
Never tested, nor do I care if it works or not since I'm not using Windows. <br>


