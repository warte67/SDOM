// SDOM_Stage.hpp

#pragma once

#include <SDOM/SDOM_IDisplayObject.hpp>

namespace SDOM
{
    struct InitStageObject : public InitDisplayObject
    {
        InitStageObject() : InitDisplayObject() 
        { 
            name = "mainStage"; 
            color = {0, 0, 0, 255}; 
        }
    };

    class Factory; // Forward declaration

    class Stage : public IDisplayObject
    {
        friend class Factory; // Allow Factory to create Stage Objects
        friend class Core;    // Allow Core to access Factory internals if needed

    protected:   // protected constructor so only the Factory can create the object
        Stage(const InitStageObject& init); ///< Constructor using InitStageObject structure
        Stage(const Json& config);          ///< Constructor using JSON configuration

    public:

        /**
         * @brief Static creator function for Factory registration.
         * @param config JSON configuration for the Stage object.
         * @return Unique pointer to the created Stage object.
         */
        static std::unique_ptr<IResourceObject> Creator(const Json& config) 
        {
            return std::unique_ptr<IResourceObject>(new Stage(config));
        }

        /**
         * @brief Destructor.
         * @note Constructors are private; use Factory to create instances.
         */
        virtual ~Stage() = default;

        /**
         * @name Virtual Methods
         * @brief All virtual methods for this class.
         * @{
         */

        /**
         * @brief Called during object initialization.
         * @return True if initialization succeeds, false otherwise.
         */
        virtual bool onInit() override;             ///< Called when the display object is initialized
                
        /**
         * @brief Called during object shutdown.
         */
        virtual void onQuit() override;             ///< Called when the display object is being destroyed

        /**
         * @brief Called every frame to update the display object
         * 
         * @param fElapsedTime Amount of time since the last update (in seconds)
         */
        virtual void onUpdate(float fElapsedTime);  ///< Called every frame to update the display object

        /**
         * @brief Called when an event occurs.
         * 
         * @param event The event that occurred.
         */
        virtual void onEvent(const Event& event);   ///< Called when an event occurs

        /**
         * @brief Called to render the display object.
         */
        virtual void onRender();                    ///< Called to render the display object

        /**
         * @brief Runs unit tests for this data object.
         * @details
         * Called during startup or explicit unit test runs. Each object should validate its own state and behavior.
         * @return true if all tests pass, false otherwise.
         */
        virtual bool onUnitTest();                  ///< Called during startup or explicit unit test runs.
        /** @} */



    };

} // namespace SDOM