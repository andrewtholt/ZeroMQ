#include <iostream>  // For standard input/output (cout, cerr)
#include <string>    // For std::string
#include <vector>    // For std::vector (useful for JSON arrays)
#include "nlohmann/json.hpp"  // Include the nlohmann/json single-header file

// It's common practice to use a 'using' declaration for brevity
// if you're going to use the json type extensively.
using json = nlohmann::json;

int main() {
    std::cout << "--- Nlohmann JSON Sample Program ---" << std::endl;

    // --- Part 1: Parsing JSON from a string ---
    std::cout << "\n--- Part 1: Parsing JSON ---" << std::endl;
    std::string json_data_string = R"({
        "book": {
            "title": "The Hitchhiker's Guide to the Galaxy",
            "author": "Douglas Adams",
            "rating": 4.8,
            "published_year": 1979,
            "genres": ["Science Fiction", "Comedy", "Absurdist Fiction"],
            "is_available": true,
            "publisher": null,
            "reviews": [
                {"user": "Alice", "score": 5, "comment": "Absolutely brilliant!"},
                {"user": "Bob", "score": 4, "comment": "Funny and thought-provoking."}
            ]
        },
        "metadata": {
            "version": "1.0",
            "date_created": "2025-08-04"
        }
    })";

    try {
        json parsed_json = json::parse(json_data_string);
        std::cout << "Successfully parsed JSON:" << std::endl;
        std::cout << parsed_json.dump(2) << std::endl; // Pretty-print with 2 spaces indent
    } catch (const json::parse_error& e) {
        std::cerr << "JSON Parse Error: " << e.what() << std::endl;
        return 1; // Indicate an error
    }

    // Assign the parsed JSON to a variable we'll use throughout
    json book_data = json::parse(json_data_string);

    // --- Part 2: Accessing JSON Elements ---
    std::cout << "\n--- Part 2: Accessing Elements ---" << std::endl;

    // Accessing primitive values
    std::string title = book_data["book"]["title"];
    std::string author = book_data["book"]["author"];
    double rating = book_data["book"]["rating"];
    int year = book_data["book"]["published_year"];
    bool available = book_data["book"]["is_available"];

    std::cout << "Book Title: " << title << std::endl;
    std::cout << "Author: " << author << std::endl;
    std::cout << "Rating: " << rating << std::endl;
    std::cout << "Published Year: " << year << std::endl;
    std::cout << "Is Available: " << (available ? "Yes" : "No") << std::endl;

    // Accessing elements safely with .at() (throws exception if not found)
    try {
        std::string non_existent = book_data.at("book").at("non_existent_key");
        std::cout << "This should not print: " << non_existent << std::endl;
    } catch (const json::out_of_range& e) {
        std::cerr << "Caught expected error when accessing non-existent key: " << e.what() << std::endl;
    }

    // Accessing elements with default values using .value()
    std::string publisher_name = book_data["book"].value("publisher", "Unknown Publisher"); // "publisher" is null, so "Unknown Publisher"
    std::cout << "Publisher: " << publisher_name << std::endl;

    std::string series_name = book_data["book"].value("series", "Not part of a series"); // "series" doesn't exist
    std::cout << "Series: " << series_name << std::endl;

    // Accessing array elements
    std::cout << "First genre: " << book_data["book"]["genres"][0] << std::endl;
    std::cout << "Second genre: " << book_data["book"]["genres"].at(1) << std::endl;

    // --- Part 3: Iterating over JSON Arrays and Objects ---
    std::cout << "\n--- Part 3: Iterating ---" << std::endl;

    // Iterate over "genres" array
    std::cout << "Genres:" << std::endl;
    for (const auto& genre : book_data["book"]["genres"]) {
        std::cout << "- " << genre << std::endl;
    }

    // Iterate over "reviews" array (each element is an object)
    std::cout << "\nReviews:" << std::endl;
    for (const auto& review : book_data["book"]["reviews"]) {
        std::cout << "  User: " << review["user"]
                  << ", Score: " << review["score"]
                  << ", Comment: \"" << review["comment"] << "\"" << std::endl;
    }

    // Iterate over "metadata" object (key-value pairs)
    std::cout << "\nMetadata:" << std::endl;
    // Using C++17 structured bindings for clean key/value access
    for (auto& [key, value] : book_data["metadata"].items()) {
        std::cout << "  Key: " << key << ", Value: " << value << std::endl;
    }


    // --- Part 4: Modifying JSON Data ---
    std::cout << "\n--- Part 4: Modifying JSON ---" << std::endl;

    // Modify an existing value
    book_data["book"]["rating"] = 4.9;
    std::cout << "Updated Rating: " << book_data["book"]["rating"] << std::endl;

    // Add a new key-value pair to an object
    book_data["book"]["isbn"] = "978-0345391803";
    std::cout << "Added ISBN: " << book_data["book"]["isbn"] << std::endl;

    // Add an element to an array
    book_data["book"]["genres"].push_back("Adventure");
    std::cout << "Added 'Adventure' to genres:" << std::endl;
    for (const auto& genre : book_data["book"]["genres"]) {
        std::cout << "- " << genre << std::endl;
    }

    // Remove an element from an array by index
    // Note: nlohmann/json arrays are like std::vector, so erasing shifts subsequent elements
    if (book_data["book"]["genres"].is_array() && book_data["book"]["genres"].size() > 0) {
        book_data["book"]["genres"].erase(book_data["book"]["genres"].begin()); // Remove first genre
        std::cout << "Genres after removing first:" << std::endl;
        for (const auto& genre : book_data["book"]["genres"]) {
            std::cout << "- " << genre << std::endl;
        }
    }

    // Remove a key-value pair from an object
    book_data["metadata"].erase("date_created");
    std::cout << "Metadata after removing 'date_created':" << std::endl;
    std::cout << book_data["metadata"].dump(2) << std::endl;

    // --- Part 5: Creating JSON from Scratch ---
    std::cout << "\n--- Part 5: Creating JSON from Scratch ---" << std::endl;

    json new_item;

    // Creating an object
    new_item["product_id"] = "ABC-123";
    new_item["name"] = "Wireless Earbuds";
    new_item["price"] = 79.99;
    new_item["in_stock"] = true;
    new_item["features"] = json::array({"Noise Cancelling", "Bluetooth 5.0", "Water Resistant"});

    // Creating a nested object
    new_item["manufacturer"]["company"] = "AudioTech Inc.";
    new_item["manufacturer"]["country"] = "USA";

    // Adding an empty array or object explicitly
    new_item["related_products"] = json::array();
    new_item["notes"] = json::object();

    std::cout << "Newly created JSON object:\n" << new_item.dump(4) << std::endl;

    // --- Part 6: Converting C++ types to JSON (Implicit Conversion) ---
    std::cout << "\n--- Part 6: Type Conversions ---" << std::endl;

    std::vector<int> numbers = {10, 20, 30};
    json json_numbers = numbers; // std::vector<int> automatically converts to JSON array
    std::cout << "std::vector<int> as JSON: " << json_numbers << std::endl;

    std::map<std::string, double> currency_rates = {{"USD", 1.0}, {"EUR", 0.92}, {"GBP", 0.79}};
    json json_rates = currency_rates; // std::map automatically converts to JSON object
    std::cout << "std::map<std::string, double> as JSON: " << json_rates << std::endl;

    // You can also convert from JSON to C++ types
    int retrieved_year = book_data["book"]["published_year"]; // Implicit conversion
    std::cout << "Retrieved year (int): " << retrieved_year << std::endl;

    std::vector<std::string> retrieved_genres = book_data["book"]["genres"]; // Implicit conversion to vector
    std::cout << "Retrieved genres (std::vector<std::string>):" << std::endl;
    for (const auto& g : retrieved_genres) {
        std::cout << "- " << g << std::endl;
    }


    std::cout << "\n--- Sample Program Finished ---" << std::endl;

    return 0;
}
