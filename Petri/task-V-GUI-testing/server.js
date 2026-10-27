// Imports modules for the HTTP server, file reading, and path handling
const http = require("node:http");
const fs = require("node:fs");
const path = require("node:path");

// Sets the public folder as the root directory
const root = path.join(__dirname, "public");

// Maps file extensions to their content types
const mime = {
  ".html": "text/html; charset=utf-8",
  ".js": "text/javascript; charset=utf-8",
  ".css": "text/css; charset=utf-8",
  ".json": "application/json; charset=utf-8"
};

// Creates the HTTP server
http.createServer((req, res) => {
  // Extracts and decodes the requested URL path
  const pathname = decodeURIComponent(
    new URL(req.url, "http://localhost").pathname
  );

  // Serves index.html for the root URL
  const relativePath = pathname === "/" ? "index.html" : pathname.slice(1);
  const file = path.resolve(root, relativePath);

  // Blocks requests that attempt to access files outside the public folder
  if (!file.startsWith(root + path.sep)) {
    res.writeHead(403);
    return res.end("Forbidden");
  }

  // Reads the requested file
  fs.readFile(file, (err, data) => {
    // Returns a 404 error if the file cannot be read
    if (err) {
      res.writeHead(404, { "Content-Type": "text/plain; charset=utf-8" });
      return res.end("File not found");
    }

    // Sets the response status and content type
    res.writeHead(200, {
      "Content-Type": mime[path.extname(file).toLowerCase()] ||
        "application/octet-stream"
    });

    // Sends the file contents to the browser
    res.end(data);
  });
}).listen(3000, () => {
  // Confirms that the server is running
  console.log("Open http://localhost:3000");
});
