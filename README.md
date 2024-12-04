
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

### 1. Clone the repository:

```bash
git clone https://github.com/mattsykesj/cssg.git
cd cssg
```

### 2. Add your `.md` files to the `content` folder.

### 3. Add a layout file to the `partials` folder:

- Create a `_layout.html` file in the `partials` directory.
- In the layout, insert `{{ "name_of_md.html" }}` where you want the content to be injected.

Example of the layout file:

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

### 4. Build the executable:

Run the `build_debug.bat` or `build_release.bat` to generate the executable. These batch files will use Clang to compile the code into a Windows executable. Make sure you have **Clang** installed on your machine before proceeding.

**Note:** This project currently relies on Windows-specific functions and the build system is tailored for Clang.

### 5. Generate Static HTML:

Once you’ve built the executable, you can run it to generate your static site.

For **debug** build:

```bash
./bin/debug/cssg.exe
```

For **release** build:

```bash
./bin/release/cssg.exe
```

The program will take the content from your Markdown files, apply the layout from `_layout.html`, and output the HTML files to the `public` folder (e.g., `index.html`).

---

## Limitations

- **Windows-only**: This project is currently designed for Windows and relies on Windows-specific file path handling and functions.
- **Clang**: The `build_debug.bat` and `build_release.bat` files use **Clang** as the compiler. If you are working on a different platform or using a different compiler, you'll need to adjust the build process.
- **Features in Progress**: The parser currently only supports **headers**, **bold**, and **italics**. Additional Markdown features will be added in future updates.

---

## Additional Notes

The program currently requires running from the root directory of your project (where the `config.toml` file is located) in order to work properly. You may need to ensure you’re executing the program from the right place.
