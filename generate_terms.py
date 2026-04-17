#!/usr/bin/env python3
"""Called from Nerd Snake with a topic. Runs built generate_terms if available; otherwise writes default terms.json."""

import json
import os
import subprocess
import sys

DEFAULT_TERMS = [
    {"word": "RNA", "definition": "Ribonucleic acid involved in gene expression."},
    {"word": "miRNA", "definition": "MicroRNA regulating gene expression."},
    {"word": "siRNA", "definition": "Small interfering RNA that silences genes."},
    {"word": "DNA", "definition": "Molecule carrying genetic instructions."},
    {"word": "Neuron", "definition": "A nerve cell that transmits signals."},
    {"word": "Genome", "definition": "The complete set of genetic material."},
    {"word": "Ribosome", "definition": "Cell structure that synthesizes proteins."},
    {"word": "Enzyme", "definition": "Protein that accelerates chemical reactions."},
    {"word": "Mutation", "definition": "Permanent change in DNA sequence."},
    {"word": "Protein", "definition": "Large molecule made of amino acids."},
]


def main() -> None:
    topic = " ".join(sys.argv[1:]).strip() or "science"
    root = os.path.dirname(os.path.abspath(__file__))
    out_path = os.path.join(root, "terms.json")
    candidates = [
        os.path.join(root, "build", "bin", "Release", "generate_terms.exe"),
        os.path.join(root, "build", "bin", "Debug", "generate_terms.exe"),
        os.path.join(root, "build", "bin", "generate_terms.exe"),
        os.path.join(root, "build", "bin", "generate_terms"),
    ]

    for exe in candidates:
        if os.path.isfile(exe):
            try:
                subprocess.run([exe, topic], cwd=root, check=True)
                return
            except (subprocess.CalledProcessError, OSError):
                break

    with open(out_path, "w", encoding="utf-8") as f:
        json.dump(DEFAULT_TERMS, f, indent=2)


if __name__ == "__main__":
    main()
#!/usr/bin/env python3
"""Called from Nerd Snake with a topic. Runs build/bin/generate_terms if available; otherwise writes default terms.json."""

import json
import os
import subprocess
import sys

DEFAULT_TERMS = [
    {"word": "RNA", "definition": "Ribonucleic acid involved in gene expression."},
    {"word": "miRNA", "definition": "MicroRNA regulating gene expression."},
    {"word": "siRNA", "definition": "Small interfering RNA that silences genes."},
    {"word": "DNA", "definition": "Molecule carrying genetic instructions."},
    {"word": "Neuron", "definition": "A nerve cell that transmits signals."},
    {"word": "Genome", "definition": "The complete set of genetic material."},
    {"word": "Ribosome", "definition": "Cell structure that synthesizes proteins."},
    {"word": "Enzyme", "definition": "Protein that accelerates chemical reactions."},
    {"word": "Mutation", "definition": "Permanent change in DNA sequence."},
    {"word": "Protein", "definition": "Large molecule made of amino acids."},
]


def main() -> None:
    topic = " ".join(sys.argv[1:]).strip() or "science"
    root = os.path.dirname(os.path.abspath(__file__))
    out_path = os.path.join(root, "terms.json")
    candidates = [
        os.path.join(root, "build", "bin", "Release", "generate_terms.exe"),
        os.path.join(root, "build", "bin", "Debug", "generate_terms.exe"),
        os.path.join(root, "build", "bin", "generate_terms.exe"),
        os.path.join(root, "build", "bin", "generate_terms"),
    ]

    for exe in candidates:
        if os.path.isfile(exe):
            try:
                subprocess.run([exe, topic], cwd=root, check=True)
                return
            except (subprocess.CalledProcessError, OSError):
                break
    with open(out_path, "w", encoding="utf-8") as f:
        json.dump(DEFAULT_TERMS, f, indent=2)


if __name__ == "__main__":
    main()
