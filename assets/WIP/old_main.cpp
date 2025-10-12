// main.cpp
//
// fancy text: https://patorjk.com/software/taag/#p=display&f=Ivrit&t=SDOM_Stage.hpp
// 

#include "SDOM_Types.hpp"


SDOM::resource_ptr<SDOM::DisplayObject> g_stage;

void userInit(SDOM::Stage& stage) {
    std::cout << CLR::WHITE << "User-defined initialization logic." << std::endl;

    // // Create child nodes
    // auto child1 = SDOM::make_resource<SDOM::DisplayObject>("Child1", 10,10,50,25);
    // child1->color = { 0, 255, 255, 255 }; // Set color to cyan
    // auto child2 = SDOM::make_resource<SDOM::DisplayObject>("Child2", 100,120,75,50);
    // child2->color = { 0, 255, 0, 255 }; // Set color to green

    // // Add children to the stage
    // stage.addChild(child1);
    // stage.addChild(child2);

    // // Create grandchildren for child2
    // auto grandchild1 = SDOM::make_resource<SDOM::DisplayObject>("Grandchild1", 10,10,25,25);
    // grandchild1->color = { 255, 0, 0, 255 }; // Set color to red
    // auto grandchild2 = SDOM::make_resource<SDOM::DisplayObject>("Grandchild2", 50,10,15,15);
    // grandchild2->color = { 255, 255, 0, 255 }; // Set color to BLUE

    // // Add grandchildren to child2
    // child2->addChild(grandchild1);
    // child2->addChild(grandchild2);

    // std:: cout << CLR::BLUE << "\nInitial Tree Structure:\n" << CLR::YELLOW;  
    // stage.printTree();  
    // std::cout << std::endl;      


    // // TEST #1
    // std::cout << CLR::BLUE << "Test #1" << std::endl;
    // child1->setPriority(10);     std::cout << CLR::GREEN << "child1->setPriority(10);\n";
    // child2->setPriority(5);    std::cout << CLR::GREEN << "child2->setPriority(5);\n";
    // std::cout << CLR::YELLOW;
    // stage.printTree();  
    // std::cout << CLR::GREEN;
    // std::cout << "Stage->children Priorities: ";
    // std::vector<int> childPriorities = stage.getChildrenPriorities();
    // for (int priority : childPriorities) {
    //     std::cout << priority << " ";
    // }    
    // std::cout << "\nchild2->children priorities: ";
    // childPriorities.clear();
    // childPriorities = child2->getChildrenPriorities();
    // for (int priority : childPriorities) {
    //     std::cout << priority << " ";
    // }    
    // std::cout << "\n" << std::endl;      

    // // TEST #2find_library(SDL3_mixer_LIB SDL3_mixer REQUIRED HINTS /usr/local/lib)

    // std::cout << CLR::BLUE << "Test #2" << std::endl;
    // grandchild1->setPriority(7);    std::cout << CLR::GREEN << "grandchild1->setPriority(7);\n";
    // grandchild2->setPriority(3);    std::cout << CLR::GREEN << "grandchild2->setPriority(3);\n";
    // std::cout << CLR::YELLOW;
    // stage.printTree();  
    // std::cout << CLR::GREEN;

    // std::cout << "child2->children Priorities: ";
    // childPriorities.clear();
    // childPriorities = child2->getChildrenPriorities();
    // for (int priority : childPriorities) {
    //     std::cout << priority << " ";
    // }    
    // std::cout<< std::endl;
    // std::cout << "stage.getMaxPriority() = " << stage.getMaxPriority() << std::endl;
    // std::cout << "stage.getMinPriority() = " << stage.getMinPriority() << std::endl;
    // std::cout << std::endl;

    // // TEST #3
    // std::cout << CLR::BLUE << "Test #3" << CLR::GREEN << std::endl;
    // child1->setToLowestPriority();  std::cout << CLR::GREEN << "child1->setToLowestPriority();\n";
    // std::cout << CLR::YELLOW;
    // stage.printTree();  
    // std::cout << CLR::GREEN;

    // // TEST #4
    // std::cout << CLR::BLUE << "Test #4" << CLR::GREEN << std::endl;
    // child1->setToHighestPriority();  std::cout << CLR::GREEN << "child1->setToHighestPriority();\n";
    // std::cout << CLR::YELLOW;
    // stage.printTree();  

    // std::cout << CLR::NORMAL;



    // // Add event listeners to child1
    // stage.addEventListener(SDOM::EventType::MouseClick, 
    //     [](const SDOM::Event& event) {
    //         std::cout << event.getPhaseString() << " phase event detected on stage!" << std::endl;
    //     }, false // useCapture
    // );

    // child1->addEventListener(SDOM::EventType::MouseDoubleClick, 
    //     [](const SDOM::Event& event) {
    //         const SDOM::MouseEvent* mouseEvent = dynamic_cast<const SDOM::MouseEvent*>(&event);
    //         if (mouseEvent) {
    //             if (mouseEvent->getPhase() == SDOM::Event::Phase::Target) {
    //                 // Debug: Print the mouse event details
    //                 std::cout << "Double Click!" << std::endl;
    //             }                
    //         }
    //     } , false // useCapture
    // );
    // child1->addEventListener(SDOM::EventType::MouseClick, 
    //     [](const SDOM::Event& event) {
    //         const SDOM::MouseEvent* mouseEvent = dynamic_cast<const SDOM::MouseEvent*>(&event);
    //         if (mouseEvent) {
    //             if (mouseEvent->getPhase() == SDOM::Event::Phase::Target) {
    //                 // Debug: Print the mouse event details
    //                 std::cout << "Click!" << std::endl;
    //             }                
    //         }
    //     } , false // useCapture
    // );
    
    // // // Print the tree structure
    // // stage.printTree();    
}

void userQuit(SDOM::Stage& stage) {
    std::cout << CLR::WHITE << "\n\nUser-defined quit logic." << std::endl;

    // // // Debug: Print the number of children in the stage
    // // std::cout << "Number of children in stage: " << stage.getChildrenCount() << std::endl;

    // // Print the tree structure
    // std::cout << CLR::YELLOW;
    // stage.printTree();
    // std::cout << CLR::NORMAL;
}


int main()
{
    // std::cout << "SDOM Framework work in progress." << std::endl;


    // Initialize the SDOM framework
    SDOM::StageConfig config;
    config.window_width = 1280; // Default width of the window in pixels
    config.window_height = 720; // Default height of the window in pixels
    config.stage_width = 384; // Default width of the stage in pixels
    config.stage_height = 216; // Default height of the stage in pixels
    config.window_flags = SDL_WINDOW_RESIZABLE;
    config.logical_presentation_flags = SDL_LOGICAL_PRESENTATION_LETTERBOX;
    config.pixel_format = SDL_PIXELFORMAT_RGBA8888;
    config.bgnd_color = { 32, 8, 0, 255 }; // Default background color 
    config.name = "Stage";
    config.image_filename = "../assets/board.png"; // New field for image filename

    // set the user callbacks for initialization and shutdown
    SDOM::Stage stage;
    stage.setUserInitCallback(userInit);
    stage.setUserQuitCallback(userQuit);

    // Run the stage with the configuration
    stage.run(config);

    return 0;
}


