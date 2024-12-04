# Simple Static Site Generator in C (CSSG) - Dev Branch

## Introduction

This is the **development version** of the Simple Static Site Generator (SSG). It's a tool that converts Markdown files into static HTML pages for creating personal or project websites.

### Current Status:
The **parser** is **actively under development**. Currently, it only supports the following Markdown features:

- **Headers**: Converts Markdown headers (`#`, `##`) into HTML `<h1>`, `<h2>`, etc.
- **Bold Text**: Converts `**bold**` to `<strong>bold</strong>`.
- **Italics Text**: Converts `*italic*` to `<em>italic</em>`.

The parser is being continuously updated, and additional features like lists, links, images, and more will be added.

### Why This Project?

I built this because I wanted a lightweight, **minimalistic static site generator** for my weekend projects and personal websites. I didn’t want the complexity of bigger tools like Jekyll, so I started from scratch and built this simple tool.

## Features (Dev Branch)

- **Parsing Basic Markdown**: Converts headers, bold, and italics to HTML.
- **Simple Static Site Generation**: Takes Markdown files and outputs HTML pages.
- **Under Development**: Parser and features are still being worked on.

---

## Usage

1. Clone the repository.
2. Add your `.md` files to the `content` folder.
3. Add a layout file to the `partials` folder:
   - Create a `_layout.html` file in the `partials` directory.
   - In the layout, insert `{{ "name_of_md.html" }}` where you want the content to be injected.
   - Example:
     ```html
     <!-- _layout.html -->
     <html>
     <head><title>{{ title }}</title></head>
     <body>
       <header><h1>{{ title }}</h1></header>
       <main>
         {{ content.html }}
       </main>
     </body>
     </html>
     ```

4. Update the src in `main.c` to point to your root directory.
5. Modify the `build_debug.bat` or `build_release.bat` to point to the correct folders.
   - The generator will take the content from your Markdown files, apply the layout from `_layout.html`, and output the HTML files to the `public` folder (e.g., `index.html`).


Note currently the program uses the windows function for current directory and needs to be run from the root directory to work correctly.

example dev setup
```bash
git clone https://github.com/mattsykesj/cssg.git
cd cssg
./build_debug.bat
./bin/debug/cssg.exe