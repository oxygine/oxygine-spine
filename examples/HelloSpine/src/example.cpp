#include "oxygine-framework.h"
#include <functional>
#include "oxygine-spine.h"
#include "test.h"

using namespace oxygine;


void example_preinit() {}


spine::AnimationStateData* animationStateData = 0;
spine::AnimationState* animationState = 0;
spine::SkeletonData *skeletonData = 0;
spine::Atlas* atlas = 0;
spine::Skeleton *skeleton = 0;
oxspine::spSpineActor actor;


#define SPINE_JSON


//called from main.cpp
void example_init()
{
    Test::init();

    spTest test = new Test;
    
    test->addButton("Start Walk", []() {
        animationState->setAnimation(1, "walk", true);
    });

    test->addButton("Stop Walk", []() {
        animationState->setEmptyAnimation(1, 0.15f);
    });

    test->addButton("Start Shoot", []() {
        animationState->setAnimation(2, "shoot", true);
    });

    test->addButton("Stop Shoot", []() {
        animationState->setEmptyAnimation(2, 0.15f);
    });

    test->addButton("Shoot", []() {
        animationState->addAnimation(3, "shoot", false, 0.0f);
        animationState->addEmptyAnimation(3, 0.1f, 0.0f);
    });

    test->addButton("Jump", []() {
        animationState->addAnimation(4, "jump", false, 0.0f);
        animationState->addEmptyAnimation(4, 0.1f, 0.0f);
    });

    test->show();
        
    oxspine::init();


    file::buffer bf;

    atlas = new spine::Atlas("spine/spineboy/spineboy.atlas", oxspine::getTextureLoader());

#ifdef SPINE_JSON
    spine::SkeletonJson json(atlas);
    json.setScale(1);

    skeletonData = json.readSkeletonDataFile("spine/spineboy/spineboy-pro.json");
#else

    spine::SkeletonBinary binary(atlas);
    binary.setScale(1);

    skeletonData = binary.readSkeletonDataFile("spine/spineboy/spineboy-ess.skel");
#endif

    animationStateData = new spine::AnimationStateData(skeletonData);
    
    animationState = new spine::AnimationState(animationStateData);

    animationState->setAnimation(0, "idle", true);

    
    

    skeleton = new spine::Skeleton(skeletonData);
    skeleton->setScaleY(-1);


    actor = new oxspine::SpineActor();
    actor->setY(getStage()->getHeight() - 100);
    actor->setX(getStage()->getWidth() / 2);
    actor->setScale(0.5f);
    actor->setSkeleton(skeleton);
    actor->setAnimationState(animationState);

    //and add it to Stage as child
    getStage()->addChild(actor);
}


//called each frame from main.cpp
void example_update()
{
}

//called each frame from main.cpp
void example_destroy()
{
    actor = 0;

    delete skeleton;
    skeleton = 0;

    delete skeletonData;
    skeletonData = 0;

    delete animationState;
    animationState = 0;

    delete animationState;
    animationState = 0;

    delete animationStateData;
    animationStateData = 0;

    delete atlas;
    atlas = 0;

    oxspine::free();
    Test::free();
}
