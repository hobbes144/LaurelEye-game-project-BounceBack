// Notes:
//
// This will contain all the animation logic.
// Ideally we store the animation method in the config, so that we can
// change from VQS to iVQS or keyframe only or other methods in the future.
//
// During init of a component:
// Init the AnimationComponent with the bone localPose values.
//
// During setAnimation of a component:
// We set up the time related info, the animation resource being animated.
// The system will update the localPose of the AnimationComponent using the
// animation technique setup in the config. (Maybe allow this to be per comp?)
//
// During update of a component:
// Update the globalPose, then upload the data to GPU using relevant UBO.
//
// Ideally the Mesh draw will take care of the UBO bind during draw calls, so
// we could cache the UBO handle in the Mesh for efficiency.
//
// During Animation resource load:
// Create the resource with channels that map to the respective bone index.
