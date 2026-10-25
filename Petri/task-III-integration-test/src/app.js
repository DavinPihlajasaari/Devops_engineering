import express from "express";
import { hexToRgb } from "./converter.js";

const app = express();

// Handle requests to convert a hex color to RGB.
app.get("/api/convert", (req, res) => {
  const { hex } = req.query;

  // Ensure the required query parameter is provided.
  if (typeof hex !== "string") {
    return res.status(400).json({
      error: 'Query parameter "hex" is required',
    });
  }

  try {
    // Convert the hex color to RGB.
    const rgb = hexToRgb(hex);

    // Return the original hex value and its RGB equivalent.
    return res.status(200).json({
      hex,
      rgb,
    });
  } catch (error) {
    // Return a 400 response if the hex color is invalid.
    return res.status(400).json({
      error: error.message,
    });
  }
});

export default app;