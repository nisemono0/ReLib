<h1 align="center"> ReLib </h1>
<p align="center">
  Qt program to search/read my manga library <br>
  Rewrite of <a href="https://github.com/nisemono0/HLib">HLib</a> with added multithreading, tag search and autocompletion (autocompletion is WIP) <br>
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

Available namespaces: `file_hash` `title` `artist` `parody` `circle` `magazine` `event` `publisher` `tag` <br>
When using the `file_hash` namespace the rest are ignored <br>

## Dependencied (Arch Linux)
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
