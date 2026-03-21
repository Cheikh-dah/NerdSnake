#!/usr/bin/env python3
from google import genai
import json
import sys
import re
import os
from pathlib import Path

def clean_json_response(raw_text):
    """Clean markdown and extract JSON from response."""
    # Remove markdown code blocks
    cleaned = raw_text.replace("```json", "").replace("```", "").strip()
    
    # Keep all valid characters (don't filter Unicode)
    cleaned = "".join(c for c in cleaned if ord(c) >= 32 or c in "\r\n\t")
    
    # Find outermost JSON array or object (with multiline support)
    match = re.search(r"(\{[\s\S]*\}|\[[\s\S]*\])", cleaned, re.DOTALL)
    if not match:
        return None, "Could not find JSON in the response!"
    
    json_text = match.group(0)
    # Fix trailing commas
    json_text = re.sub(r",(\s*[\]}])", r"\1", json_text)
    
    return json_text, None

def main():
    if len(sys.argv) < 2:
        print("Usage: python test_gemini.py <topic>")
        sys.exit(1)

    topic = sys.argv[1]
    model = os.getenv("GEMINI_MODEL", "gemini-2.0-flash")  # Make configurable
    
    prompt = f"Generate 20 science terms about {topic} with definitions in JSON. Return ONLY a JSON array of objects with 'term' and 'definition' fields."

    try:
        client = genai.Client()
        
        response = client.models.generate_content(
            model=model,
            contents=prompt
        )
        
        if not response or not response.text:
            print("Error: Empty response from API")
            sys.exit(1)
            
    except Exception as e:
        print(f"API Error: {e}")
        sys.exit(1)

    raw = response.text
    print("=== RAW RESPONSE ===")
    print(raw)

    json_text, error = clean_json_response(raw)
    if error:
        print(error)
        sys.exit(1)

    try:
        terms = json.loads(json_text)
        
        # Save with UTF-8 encoding
        with open("terms.json", "w", encoding="utf-8") as f:
            json.dump(terms, f, indent=4, ensure_ascii=False)
            
        print("terms.json generated successfully!")
        
        if isinstance(terms, list):
            print(f"\nFirst 3 terms:")
            for t in terms[:3]:
                print(f"- {t.get('term', 'N/A')}: {t.get('definition', 'N/A')}")
                
    except json.JSONDecodeError as e:
        print(f"Failed to parse JSON: {e}")
        print("Attempted JSON:")
        print(json_text)
        sys.exit(1)

if __name__ == "__main__":
    main()
