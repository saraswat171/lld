#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <fstream>

// Abstraction for document elements
class DocumentElement {
public:
    virtual ~DocumentElement() = default;
    virtual std::string render() const = 0;
};

// Concrete implementation for text elements
class TextElement : public DocumentElement {
private:
    std::string text;
public:
    explicit TextElement(const std::string& t) : text(t) {}
    std::string render() const override {
        return text;
    }
};

// Concrete implementation for image elements
class ImageElement : public DocumentElement {
private:
    std::string imagePath;
public:
    explicit ImageElement(const std::string& path) : imagePath(path) {}
    std::string render() const override {
        return "[Image: " + imagePath + "]";
    }
};

// NewLineElement represents a line break in the document.
class NewLineElement : public DocumentElement {
public:
    std::string render() const override {
        return "\n";
    }
};

// TabSpaceElement represents a tab space in the document.
class TabSpaceElement : public DocumentElement {
public:
    std::string render() const override {
        return "\t";
    }
};

// Document class responsible for holding a collection of elements
class Document {
private:
    std::vector<std::shared_ptr<DocumentElement>> documentElements;
public:
    void addElement(std::shared_ptr<DocumentElement> element) {
        documentElements.push_back(element);
    }

    // Renders the document by concatenating the render output of all elements.
    std::string render() const {
        std::string result = "";
        for (const auto& element : documentElements) {
            result += element->render();
        }
        return result;
    }
};

// Persistence abstraction
class Persistence {
public:
    virtual ~Persistence() = default;
    virtual void save(const std::string& data) = 0;
};

// FileStorage implementation of Persistence
class FileStorage : public Persistence {
public:
    void save(const std::string& data) override {
        std::ofstream outFile("document.txt");
        if (outFile.is_open()) {
            outFile << data;
            outFile.close();
            std::cout << "Document saved to document.txt\n";
        } else {
            std::cerr << "Error: Unable to open file for writing.\n";
        }
    }
};

// Placeholder DBStorage implementation
class DBStorage : public Persistence {
public:
    void save(const std::string& data) override {
        // Save to DB
        std::cout << "Document saved to database.\n";
    }
};

// DocumentEditor class managing client interactions
class DocumentEditor {
private:
    std::shared_ptr<Document> document;
    std::shared_ptr<Persistence> storage;
    std::string renderedDocument;

public:
    DocumentEditor(std::shared_ptr<Document> doc, std::shared_ptr<Persistence> store) 
        : document(doc), storage(store), renderedDocument("") {}

    void addText(const std::string& text) {
        document->addElement(std::make_shared<TextElement>(text));
    }

    void addImage(const std::string& imagePath) {
        document->addElement(std::make_shared<ImageElement>(imagePath));
    }

    // Adds a new line to the document.
    void addNewLine() {
        document->addElement(std::make_shared<NewLineElement>());
    }

    // Adds a tab space to the document.
    void addTabSpace() {
        document->addElement(std::make_shared<TabSpaceElement>());
    }

    std::string renderDocument() {
        if (renderedDocument.empty()) {
            renderedDocument = document->render();
        }
        return renderedDocument;
    }

    void saveDocument() {
        storage->save(renderDocument());
    }
};

// Client usage example
int main() {
    auto document = std::make_shared<Document>();
    auto persistence = std::make_shared<FileStorage>();

    DocumentEditor editor(document, persistence);

    // Simulate a client using the editor with common text formatting features.
    editor.addText("Hello, world!");
    editor.addNewLine();
    editor.addText("This is a real-world document editor example.");
    editor.addNewLine();
    editor.addTabSpace();
    editor.addText("Indented text after a tab space.");
    editor.addNewLine();
    editor.addImage("picture.jpg");

    // Render and display the final document.
    std::cout << editor.renderDocument() << std::endl;

    editor.saveDocument();

    return 0;
}
