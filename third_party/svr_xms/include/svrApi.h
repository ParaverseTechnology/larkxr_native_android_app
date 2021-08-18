//=============================================================================
//! \file svrApi.h
//
//                  Copyright (c) 2018 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//==============================================================================

#ifndef _SVR_API_H_
#define _SVR_API_H_

#include <stdlib.h>
#include <jni.h>

#define SVR_MAJOR_VERSION       3
#define SVR_MINOR_VERSION       0
#define SVR_REVISION_VERSION    1

#define SVR_MAX_RENDER_LAYERS   16

enum SvrResult
{
    // Return code for success
    SVR_ERROR_NONE = 0,

    // Return code for cases that don't fall into defined error codes
    SVR_ERROR_UNKNOWN = 1,

    // Return code to indicate that SVR isn't supported, or the necessary functionality in the system drivers
    // or VR service isn't supported.
    SVR_ERROR_UNSUPPORTED,

    // Return code for calls to functions made without first calling svrInitialize
    SVR_ERROR_VRMODE_NOT_INITIALIZED,

    // Return code for calls made into functions that require VR mode to be in a started state
    SVR_ERROR_VRMODE_NOT_STARTED,

    // Return code for calls made into functions that require VR mode to be in a stopped state
    SVR_ERROR_VRMODE_NOT_STOPPED,

    // Return code for calls made into functions where the service fails or isn't accessible
    SVR_ERROR_QVR_SERVICE_UNAVAILABLE,

    // Error for any failures in JNI/Java calls made through the API
    SVR_ERROR_JAVA_ERROR,
};

//! \brief Events
enum svrEventType
{
    kEventNone = 0,
    kEventSdkServiceStarting = 1,
    kEventSdkServiceStarted = 2,
    kEventSdkServiceStopped = 3,
    kEventControllerConnecting = 4,
    kEventControllerConnected = 5,
    kEventControllerDisconnected = 6,
    kEventThermal = 7,
    kEventVrModeStarted = 8,
    kEventVrModeStopping = 9,
    kEventVrModeStopped = 10,
    kEventSensorError = 11,
    kEventMagnometerUncalibrated = 12,
    kEventBoundarySystemCollision = 13,
    kEvent6dofRelocation = 14,
    kEvent6dofWarningFeatureCount = 15,
    kEvent6dofWarningLowLight = 16,
    kEvent6dofWarningBrightLight = 17,
    kEvent6dofWarningCameraCalibration = 18
};

struct ANativeWindow;

//! \brief Simple structure to hold 2-component vector data
struct svrVector2
{
    float x,y;
};

//! \brief Simple structure to hold 3-component vector data
struct svrVector3
{
    float x,y,z;
};

//! \brief Simple structure to hold 4-component vector data
struct svrVector4
{
    float x,y,z,w;
};

//! \brief Simple structure to hold quaternion data
struct svrQuaternion
{
    float x, y, z, w;
};

//! \brief Simple structure to hold 4x4 matrix data
struct svrMatrix4
{
    float M[4][4];
};

//! \brief Enum used to indicate which eye is being used
enum svrWhichEye
{
    kLeftEye = 0,
    kRightEye,
    kNumEyes
};

//! \brief Enum used to indicate which eye to apply a render layer
enum svrEyeMask
{
    kEyeMaskLeft = 0x00000001,
    kEyeMaskRight = 0x00000002,
    kEyeMaskBoth = 0x00000003
};

//! \brief Enum used to indicate flags passed as part of a render layer
enum svrLayerFlags
{
    kLayerFlagNone       = 0x00000000,
    kLayerFlagHeadLocked = 0x00000001,
    kLayerFlagOpaque     = 0x00000002
};

enum svrColorSpace
{
    kColorSpaceLinear = 0,
    kColorSpaceSRGB = 1,
    kNumColorSpaces
};

//! \brief Structure containing the position and orientation of the head
struct svrHeadPose
{
    svrQuaternion   rotation;
    svrVector3      position;
};

//! \brief Enum used to indicate valid components of an pose state
enum svrTrackingMode
{
    kTrackingRotation = (1 << 0),
    kTrackingPosition = (1 << 1),
    kTrackingEye      = (1 << 2)
};

//! \brief Structure containing the full set of pose data
struct svrHeadPoseState
{
    svrHeadPose         pose;                   //!< Head pose
    int32_t             poseStatus;             //!< Bit field (svrTrackingMode) indicating pose status
    uint64_t            poseTimeStampNs;        //!< Time stamp in which the head pose was generated (nanoseconds)
    uint64_t            poseFetchTimeNs;        //!< Time stamp when this pose was retrieved
    uint64_t            expectedDisplayTimeNs;  //!< Expected time when this pose should be on screen (nanoseconds)
};

//! \brief Structure containing flags indicating data valididty of an eye pose
enum svrEyePoseStatus
{
    kGazePointValid             = (1 << 0),
    kGazeVectorValid            = (1 << 1),
    kEyeOpennessValid           = (1 << 2),
    kEyePupilDilationValid      = (1 << 3),
    kEyePositionGuideValid      = (1 << 4)
};

//! \brief Structure containing the eye tracking data
struct svrEyePoseState
{
    int32_t             leftEyePoseStatus;          //!< Bit field (svrEyePoseStatus) indicating left eye pose status
    int32_t             rightEyePoseStatus;         //!< Bit field (svrEyePoseStatus) indicating right eye pose status
    int32_t             combinedEyePoseStatus;      //!< Bit field (svrEyePoseStatus) indicating combined eye pose status
    
    float               leftEyeGazePoint[3];        //!< Left Eye Gaze Point
    float               rightEyeGazePoint[3];       //!< Right Eye Gaze Point
    float               combinedEyeGazePoint[3];    //!< Combined Eye Gaze Point (HMD center-eye point)

    float               leftEyeGazeVector[3];       //!< Left Eye Gaze Point
    float               rightEyeGazeVector[3];      //!< Right Eye Gaze Point
    float               combinedEyeGazeVector[3];   //!< Comnbined Eye Gaze Vector (HMD center-eye point)

    float               leftEyeOpenness;            //!< Left eye value between 0.0 and 1.0 where 1.0 means fully open and 0.0 closed.
    float               rightEyeOpenness;           //!< Right eye value between 0.0 and 1.0 where 1.0 means fully open and 0.0 closed.

    float               leftEyePupilDilation;       //!< Left eye value in millimeters indicating the pupil dilation
    float               rightEyePupilDilation;      //!< Right eye value in millimeters indicating the pupil dilation

    float               leftEyePositionGuide[3];    //!< Position of the inner corner of the left eye in meters from the HMD center-eye coordinate system's origin.
    float               rightEyePositionGuide[3];   //!< Position of the inner corner of the right eye in meters from the HMD center-eye coordinate system's origin.
};

//! \brief Enum used for indicating the CPU/GPU performance levels
//! \sa svrBeginVr, svrSetPerformanceLevels
enum svrPerfLevel
{
    kPerfSystem     = 0,            //!< System defined performance level (default)
    kPerfMinimum    = 1,            //!< Minimum performance level 
    kPerfMedium     = 2,            //!< Medium performance level 
    kPerfMaximum    = 3,            //!< Maximum performance level
    kNumPerfLevels
};

//! \brief Flags used to set SVR options
enum svrOptionFlags
{
    kProtectedContent = (1 << 0),
    kMotionAwareFrames = (1 << 1)
};

//! \brief Structure containing parameters needed to enable VR mode
//! \sa svrBeginVr, svrOptionFlags
struct svrBeginParams
{
    int32_t         mainThreadId;           //!< Thread Id of the primary render thread
    svrPerfLevel    cpuPerfLevel;           //!< Desired CPU performance level
    svrPerfLevel    gpuPerfLevel;           //!< Desired GPU performance level
    ANativeWindow*  nativeWindow;           //!< Pointer to the Android native window
    int32_t         optionFlags;            //!< Flags to specify SVR options (svrOptionFlags)
    svrColorSpace   colorSpace;             //!< Color space to utilize for the display surface
};

//! \brief Options which can be set when submitting a frame to modify the behavior of asynchronous time warp
//! \sa svrSubmitFrame
enum svrFrameOption
{
    kDisableDistortionCorrection    = ( 1 << 0 ),   //!< Disables the lens distortion correction (useful for debugging)
    kDisableReprojection            = ( 1 << 1 ),   //!< Disables re-projection
    kEnableMotionToPhoton           = ( 1 << 2 ),   //!< Enables motion to photon testing 
    kDisableChromaticCorrection     = ( 1 << 3 )   //!< Disables the lens chromatic aberration correction (performance optimization)
};

//! \brief Enum used to indicate the type of texture passed in as a render layer
//! \sa svrSubmitFrame
enum svrTextureType
{
    kTypeTexture = 0,               //!< Standard texture
    kTypeTextureArray,              //!< Standard texture array (Left eye is first layer, right eye is second layer)
    kTypeImage,                     //!< EGL Image texture
    kTypeEquiRectTexture,           //!< Equirectangular texture
    kTypeEquiRectImage,             //!< Equirectangular Image texture
    kTypeCubemapTexture,            //!< Cubemap texture (Not supporting cubemap image)
    kTypeVulkan,                    //!< Vulkan texture
};

//! \brief Information about texture if type is kTypeVulkan
//! \sa svrSubmitFrame
struct svrVulkanTexInfo
{
    uint32_t            memSize;
    uint32_t            width;
    uint32_t            height;
    uint32_t            numMips;
    uint32_t            bytesPerPixel;
    uint32_t            renderSemaphore;
};


//! \brief Enum used to indicate the type of warp/composition that should be used for a frame
enum svrWarpType
{
    kSimple                         //!< Basic single layer (world) warp 
};

//! \brief Enumeration of possible warp mesh types
//! \sa svrDeviceInfo
enum svrWarpMeshType
{
    kMeshTypeColumsLtoR = 0,    // Columns Left to Right
    kMeshTypeColumsRtoL,        // Columns Right to Left
    kMeshTypeRowsTtoB,          // Rows Top to Bottom
    kMeshTypeRowsBtoT,          // Rows Bottom to Top
};

//! \brief Enumeration of possible warp meshes
//! \sa svrSetWarpMesh, svrWarpMeshType
enum svrWarpMeshEnum
{
    kMeshEnumLeft = 0,  // Column mesh for left half of screen
    kMeshEnumRight,     // Column mesh for right half of screen
    kMeshEnumUL,        // Row mesh for Upper Left part of the screen
    kMeshEnumUR,        // Row mesh for Upper Right part of the screen
    kMeshEnumLL,        // Row mesh for Lower Left part of the screen
    kMeshEnumLR,        // Row mesh for Lower Right part of the screen
    kWarpMeshCount
};

//! \brief Render layer screen position and UV coordinates
//! \sa svrSubmitFrame
struct svrLayoutCoords
{
    float               LowerLeftPos[4];                        //!< 0 = X-Position; 1 = Y-Position; 2 = Z-Position; 3 = W-Component
    float               LowerRightPos[4];                       //!< 0 = X-Position; 1 = Y-Position; 2 = Z-Position; 3 = W-Component
    float               UpperLeftPos[4];                        //!< 0 = X-Position; 1 = Y-Position; 2 = Z-Position; 3 = W-Component
    float               UpperRightPos[4];                       //!< 0 = X-Position; 1 = Y-Position; 2 = Z-Position; 3 = W-Component

    float               LowerUVs[4];                            //!< [0,1] = Lower Left UV values; [2,3] = Lower Right UV values
    float               UpperUVs[4];                            //!< [0,1] = Upper Left UV values; [2,3] = Upper Right UV values

    float               TransformMatrix[16];                    //!< Column major uv transform matrix data. Applies to video textures (see SurfaceTexture::getTransformMatrix())
};

//! \brief Description of render layers
//! \sa svrSubmitFrame
struct svrRenderLayer
{
    int32_t             imageHandle;        //!< Handle to the texture/image to be rendered
    svrTextureType      imageType;          //!< Type of texture: Standard Texture or EGL Image
    svrLayoutCoords     imageCoords;        //!< Layout of this layer on the screen
    svrEyeMask          eyeMask;            //!< Determines which eye[s] receive this render layer
    uint32_t            layerFlags;         //!< Flags applied to this render layer
    svrVulkanTexInfo    vulkanInfo;         //!< Information about the data if it is a Vulkan texture
};


//! \brief Per-frame data needed for time warp, distortion/aberration correction
//! \sa svrSubmitFrame
struct svrFrameParams
{
    int32_t             frameIndex;                             //!< Frame Index
    int32_t             minVsyncs;                              //!< Minimum number of vysnc events before displaying the frame (1=display refresh, 2=half refresh, etc...)

    svrRenderLayer      renderLayers[SVR_MAX_RENDER_LAYERS];    //!< Description of each render layer

    uint32_t            frameOptions;                           //!< Options for adjusting the frame warp behavior (bitfield of svrFrameOption)
    svrHeadPoseState    headPoseState;                          //!< Head pose state used to generate the frame  
    svrWarpType         warpType;                               //!< Type of warp to be used on the frame
    float               fieldOfView;                            //!< Field of view used to generate this frame (larger than device fov to provide timewarp margin)
};

//! \brief Initialization parameters that are constant over the life-cycle of the application
//! \sa svrInitialize
struct svrInitParams
{
    JavaVM*         javaVm;                 //!< Java Virtual Machine pointer
    JNIEnv*         javaEnv;                //!< Java Environment
    jobject         javaActivityObject;     //!< Reference to the Android activity
};

//! \brief View Frustum.  These values are based on physical device properties, except the far plane is arbitrary
//! \sa svrSubmitFrame
struct svrViewFrustum
{
    float               left;           //!< Left Plane of Frustum
    float               right;          //!< Right Plane of Frustum
    float               top;            //!< Top Plane of Frustum
    float               bottom;         //!< Bottom Plane of Frustum

    float               near;           //!< Near Plane of Frustum
    float               far;            //!< Far Plane of Frustum (Arbitrary)
};

struct svrFoveation
{
    svrVector2 gain;             //!< Foveation Gain Rate [1, ...]
    float area;                  //!< Foveation Area Size [0, ...]
    float minimum;               //!< Foveation Minimum Resolution [1, 1/2, 1/4, ..., 1/16, 0]
};

//! \brief Basic device information to allow the client code to optimally setup their simulation and rendering pipelines
struct svrDeviceInfo
{
    int32_t         displayWidthPixels;         //!< Physical width of the display (pixels)
    int32_t         displayHeightPixels;        //!< Physical height of the display (pixels)
    float           displayRefreshRateHz;       //!< Refresh rate of the display
    int32_t         displayOrientation;         //!< Display orientation (degrees at initialization - 0,90,180,270)
    int32_t         targetEyeWidthPixels;       //!< Recommended eye buffer width (pixels)
    int32_t         targetEyeHeightPixels;      //!< Recommended eye buffer height (pixels)
    float           targetFovXRad;              //!< Recommended horizontal FOV
    float           targetFovYRad;              //!< Recommended vertical FOV
    svrViewFrustum  leftEyeFrustum;             //!< Recommended Frustum information for left eye
    svrViewFrustum  rightEyeFrustum;            //!< Recommended Frustum information for right eye
    float           targetEyeConvergence;       //!< Recommended eye convergence
    float           targetEyePitch;             //!< Recommended eye pitch
    int32_t         deviceOSVersion;            //!< Android OS Version of the device
    svrWarpMeshType warpMeshType;               //!< Type of mesh used to render eye buffer
    svrFoveation    lowFoveation;               //!< Low foveation values
    svrFoveation    medFoveation;               //!< Medium foveation values
    svrFoveation    highFoveation;              //!< High foveation values
};

enum svrThermalLevel
{
    kSafe,
    kLevel1,
    kLevel2,
    kLevel3,
    kCritical,
    kNumThermalLevels
};

enum svrThermalZone
{
    kCpu,
    kGpu,
    kSkin,
    kNumThermalZones
};

struct svrEventData_Thermal
{
    svrThermalZone  zone;               //!< Thermal zone
    svrThermalLevel level;              //!< Indication of the current zone thermal level
};

typedef union 
{
    svrEventData_Thermal    thermal;
    uint32_t                data[2];
} svrEventData;

struct svrEvent
{
    svrEventType    eventType;              //!< Type of event
    uint32_t        deviceId;               //!< An identifier for the device that generated the event (0 == HMD)
    float           eventTimeStamp;         //!< Time stamp for the event in seconds since the last svrBeginVr call
    svrEventData    eventData;              //!< Event specific data payload
};
//////////////////////////////////////////////////////////////////////////
/// \struct svrSensorRawData
/// \brief Represents the raw data of the IMU sensor
struct svrSensorRawData {
    uint64_t gts;
    uint64_t ats;
    float gx;
    float gy;
    float gz;
    float ax;
    float ay;
    float az;
    uint8_t reserved[24];
} ;


//! \brief Events to use in svrControllerSendMessage
enum svrControllerMessageType
{
    kControllerMessageRecenter = 0,
    kControllerMessageVibration = 1
};

//! \brief Query Values
enum svrControllerQueryType
{
    kControllerQueryBatteryRemaining = 0,
    kControllerQueryControllerCaps = 1
};

//! Controller Connection state
enum svrControllerConnectionState {
    kNotInitialized = 0,
    kDisconnected = 1,
    kConnected = 2,
    kConnecting = 3,
    kError = 4
};

//! Controller Touch button enumerations
namespace svrControllerTouch {
  enum {
    None                = 0x00000000,
    One                 = 0x00000001,
    Two                 = 0x00000002,
    Three               = 0x00000004,
    Four                = 0x00000008,
    PrimaryThumbstick   = 0x00000010,
    SecondaryThumstick  = 0x00000020,
    Any                 = ~None
  };
}

//! Controller Trigger enumerations
namespace svrControllerAxis1D {
enum {
  PrimaryIndexTrigger   = 0x00000000,
  SecondaryIndexTrigger = 0x00000001,
  PrimaryHandTrigger    = 0x00000002,
  SecondaryHandTrigger  = 0x00000003
};
}

//! Controller Joystick enumerations
namespace svrControllerAxis2D {
enum {
  PrimaryThumbstick     = 0x00000000,
  SecondaryThumbstick   = 0x00000001
};
}

//! Controller Button enumerations
namespace svrControllerButton {
enum {
  None                    = 0x00000000,
  One                     = 0x00000001,
  Two                     = 0x00000002,
  Three                   = 0x00000004,
  Four                    = 0x00000008,
  DpadUp                  = 0x00000010,
  DpadDown                = 0x00000020,
  DpadLeft                = 0x00000040,
  DpadRight               = 0x00000080,
  Start                   = 0x00000100,
  Back                    = 0x00000200,
  PrimaryShoulder         = 0x00001000,
  PrimaryIndexTrigger     = 0x00002000,
  PrimaryHandTrigger      = 0x00004000,
  PrimaryThumbstick       = 0x00008000,
  PrimaryThumbstickUp     = 0x00010000,
  PrimaryThumbstickDown   = 0x00020000,
  PrimaryThumbstickLeft   = 0x00040000,
  PrimaryThumbstickRight  = 0x00080000,
  SecondaryShoulder       = 0x00100000,
  SecondaryIndexTrigger   = 0x00200000,
  SecondaryHandTrigger    = 0x00400000,
  SecondaryThumbstick     = 0x00800000,
  SecondaryThumbstickUp   = 0x01000000,
  SecondaryThumbstickDown = 0x02000000,
  SecondaryThumbstickLeft = 0x04000000,
  SecondaryThumbstickRight = 0x08000000,
  Up                      = 0x10000000,
  Down                    = 0x20000000,
  Left                    = 0x40000000,
  Right                   = 0x80000000,
  Any                     = ~None
};
}

// Current state of the controller
struct svrControllerState {
    //! Orientation
    svrQuaternion   rotation;

    //! Position
    svrVector3      position;
    
    //! Gyro
    svrVector3      gyroscope;
    
    //! Accelerometer
    svrVector3      accelerometer;
    
    //! timestamp
    uint64_t        timestamp;

    //! All digital button states as bitflags
    uint32_t        buttonState;   //!< all digital as bit flags
    
    //! Touchpads, Joysticks
    svrVector2      analog2D[4];       //!< analog 2D's

    //! Triggers
    float           analog1D[8];       //!< analog 1D's

    //! Whether the touchpad area is being touched.
    uint32_t        isTouching;
     
    //! Controller Connection state
    svrControllerConnectionState connectionState;
};

// Caps of the Controller.
struct svrControllerCaps {
    //! Device Manufacturer
    char deviceManufacturer[64];
    
    //! Device Identifier from the module
    char deviceIdentifier[64];
    
    //! Controller Capabilities
    uint32_t caps; //0 bit = Provides Rotation; 1 bit = Position;

    //! Enabled Buttons Bitfield
    uint32_t activeButtons;
    
    //! Active 2D Analogs Bitfield
    uint32_t active2DAnalogs;
    
    //! Active 1D Analogs Bitfield
    uint32_t active1DAnalogs;
    
    //! Active Touch Buttons
    uint32_t activeTouchButtons;
};

#ifndef SVRP_EXPORT
    #define SVRP_EXPORT
#endif

#ifdef __cplusplus 
extern "C" {
#endif

//! \brief Returns the VR SDK version string
SVRP_EXPORT const char* svrGetVersion();

//! \brief Returns the VR service version string
SVRP_EXPORT SvrResult svrGetVrServiceVersion(char *pRetBuffer, int bufferSize);

//! \brief Returns the VR client version string
SVRP_EXPORT SvrResult svrGetVrClientVersion(char *pRetBuffer, int bufferSize);

//! \brief Initializes VR components 
//! \param pInitParams svrInitParams structure
SVRP_EXPORT SvrResult svrInitialize(const svrInitParams* pInitParams);

//! \brief Releases VR components
SVRP_EXPORT SvrResult svrShutdown();

//! \brief Queries for device specific information
//! \return svrDeviceInfo structure containing device specific information (resolution, fov, etc..)
SVRP_EXPORT svrDeviceInfo   svrGetDeviceInfo();

//! \brief Requests specific brackets of CPU/GPU performance
//! \param cpuPerfLevel Requested performance level for CPU
//! \param gpuPerfLevel Requested performance level for GPU
SVRP_EXPORT SvrResult svrSetPerformanceLevels(svrPerfLevel cpuPerfLevel, svrPerfLevel gpuPerfLevel);

//! \brief Enables VR services
//! \param pBeginParams svrBeginParams structure
SVRP_EXPORT SvrResult svrBeginVr(const svrBeginParams* pBeginParams);

//! \brief Disables VR services
SVRP_EXPORT SvrResult svrEndVr();

//! \brief Reset VR services
SVRP_EXPORT SvrResult svrResetVr();

//! \brief Calculates a predicted time when the current frame would be displayed. Assumes a pipeline depth of 1
//! \return Predicted display time for the current frame in milliseconds
SVRP_EXPORT float svrGetPredictedDisplayTime();

//! \brief Calculates a predicted time when the current frame would be displayed.  Allows for specifying the current depth of the pipeline
//! \return Predicted display time for the current frame in milliseconds
SVRP_EXPORT float svrGetPredictedDisplayTimePipelined(unsigned int depth);

//! \brief Calculates a predicted head pose
//! \param predictedTimeMs Time ahead of the current time in ms to predict a head pose for
//! \return The predicted head pose and relevant pose state information
SVRP_EXPORT svrHeadPoseState svrGetPredictedHeadPose( float predictedTimeMs );

//! \brief Retrieves a historic head pose
//! \param predictedTimeMs Time in ns to retrieve a head pose for
//! \return The head pose and relevant pose state information
SVRP_EXPORT svrHeadPoseState svrGetHistoricHeadPose(int64_t timestampNs);

//! \brief Retrieves a current eye pose
//! \return The eye pose
SVRP_EXPORT SvrResult svrGetEyePose(svrEyePoseState *pReturnPose);

//! \brief Recenters the head position and orientation at the current values
SVRP_EXPORT SvrResult svrRecenterPose(bool YawOnly);

//! \brief Recenters the head position at the current position
SVRP_EXPORT SvrResult svrRecenterPosition();

//! \brief Recenters the head orientation (Yaw only) at the current value
SVRP_EXPORT SvrResult svrRecenterOrientation(bool yawOnly=true);

//! \brief Returns the supported tracking types
//! \return Bitfield of svrTrackingType values indicating the supported tracking modes
SVRP_EXPORT uint32_t svrGetSupportedTrackingModes();

//! \brief Sets the current head tracking mode
//! \param trackingModes Bitfield of svrTrackingType values indicating the tracking modes to enable
SVRP_EXPORT SvrResult svrSetTrackingMode(uint32_t trackingModes);

//! \brief Returns the current head tracking mode
//! \return trackingMode Bitfield of svrTrackingType values indicating the tracking modes enabled
SVRP_EXPORT uint32_t svrGetTrackingMode();

//! \brief Polls for an available event.  If event(s) are present pEvent will be filled with the event details
//! \return true if an event was present, false if not
SVRP_EXPORT bool svrPollEvent(svrEvent *pEvent);

//! \brief Called after eye buffer is bound but before game rendering starts
//! \param whichEye Which eye is being rendered
//! \param imageType Type of eye render buffer
SVRP_EXPORT SvrResult svrBeginEye(svrWhichEye whichEye, svrTextureType imageType = kTypeTexture);

//! \brief Called after eye buffer is rendered but before frame is submitted
//! \param whichEye Which eye is being rendered
//! \param imageType Type of eye render buffer
SVRP_EXPORT SvrResult svrEndEye(svrWhichEye whichEye, svrTextureType imageType = kTypeTexture);

//! \brief Submits a frame to asynchronous time warp
//! \param pFrameParams svrFrameParams structure
SVRP_EXPORT SvrResult svrSubmitFrame(const svrFrameParams* pFrameParams);

//! \brief Starts Tracking 
//! \param controllerDesc Controller Description
//! \return handle for the controller
SVRP_EXPORT int svrControllerStartTracking(const char* controllerDesc);

//! \brief Stops tracking the controller
//! \param controllerHandle handle for the controller
SVRP_EXPORT void svrControllerStopTracking(int controllerHandle);

//! \brief Get the current state of the controller
//! \param controllerHandle handle for the controller
SVRP_EXPORT svrControllerState svrControllerGetState(int controllerHandle, int controllerSpace = 0);

//! \brief Send a message to the controller
//! \param controllerHandle handle for the controller
//! \param what type of event
//! \param arg1 argument for the event
//! \param arg2 argument for the event
SVRP_EXPORT void svrControllerSendMessage(int controllerHandle, int what, int arg1, int arg2);

//! \brief make a query to the controller
//! \param controllerHandle handle for the controller
//! \param what info
//! \param memory memory to fill in
//! \param memorySize size of the memory to fill in
//! \return number of bytes written into the memory
SVRP_EXPORT int svrControllerQuery(int controllerHandle, int what, void* memory, unsigned int memorySize);

//! \brief Replaces the current Time Warp mesh the supplied mesh
//! \param whichMesh Which mesh is to be replaced
//! \param pVertexData 15 float values per vertex: Position = 3; R|G|B UV values 4 each (Fourth value is Vignette multiplier [0,1]). NULL reverts the override.
//! \param vertexSize Size of pVertexData
//! \param nVertices Number of vertices contained in pVertexData
//! \param pIndices Pointer to indice data. Expected to be used with GL_TRIANGLES
//! \param nIndices Number of indices contained in pIndices. Expected to be used with GL_TRIANGLES
//! \return SVR_ERROR_NONE for success; SVR_ERROR_UNSUPPORTED if vertexSize is incorrect; SVR_ERROR_UNKNOWN for any other error
//! \sa svrWarpMeshType, svrWarpMeshEnum
SVRP_EXPORT SvrResult svrSetWarpMesh(svrWarpMeshEnum whichMesh, void *pVertexData, int vertexSize, int nVertices, unsigned int* pIndices, int nIndices);

//! \brief Gets triangle data for the occlusion area mesh
//! \param whichEye which eye to get the occlusion mesh for
//! \param nTriangleCount the number of triangles needed for the mesh, set to required size if pTriangles == NULL
//! \param pTriangles buffer to store occlusion mesh data
//! \return SVR_ERROR_NONE for success; SVR_ERROR_UNKNOWN for any other error
SVRP_EXPORT SvrResult svrGetOcclusionMesh(svrWhichEye whichEye, int *pTriangleCount, int *pVertexStride, float *pTriangles);

//! \brief Check whether the Boundary system is enabled
//! \return True/False for Enabled/Disabled
SVRP_EXPORT bool svrIsBoundarySystemEnabled();

//! \brief Enable/Disable the Boundary system
//! \param enableBoundary Whether to enable or disable the Boundary system
SVRP_EXPORT void svrEnableBoundarySystem(bool enableBoundary = true);

//! \brief Enable/Disable the forced display of Boundary system
//! \param enableBoundary Whether to enable or disable the forced display of Boundary system
SVRP_EXPORT void svrForceDisplayBoundarySystem(bool forceDisplayBoundary = true);

//! \brief Set parameters for the Boundary system
//! \param pMinValues Real world minimum physical boundaries (Meters. 0 = X-Position; 1 = Y-Position; 2 = Z-Position)
//! \param pMaxValues Real world maximum physical boundaries (Meters. 0 = X-Position; 1 = Y-Position; 2 = Z-Position)
//! \param visibilityRadius Distance from physical boundaries when visual notification starts to appear
//! \return SVR_ERROR_NONE for success; SVR_ERROR_UNKNOWN for any other error
SVRP_EXPORT SvrResult svrSetBoundaryParameters(float *pMinValues, float *pMaxValues, float visibilityRadius);

//! \brief Get parameters for the Boundary system
//! \param pMinValues Real world minimum physical boundaries (Meters. 0 = X-Position; 1 = Y-Position; 2 = Z-Position)
//! \param pMaxValues Real world maximum physical boundaries (Meters. 0 = X-Position; 1 = Y-Position; 2 = Z-Position)
//! \param pVisibilityRadius Distance from physical boundaries when visual notification starts to appear
//! \return SVR_ERROR_NONE for success; SVR_ERROR_UNKNOWN for any other error
SVRP_EXPORT SvrResult svrGetBoundaryParameters(float *pMinValues, float *pMaxValues, float *pVisibilityRadius);

SVRP_EXPORT void SvrUpdateWarpMeshAR (float offsetXL,float offsetXR, float offsetY);

SVRP_EXPORT void SvrGetWarpMeshAROffset (float* offsetX, float* offsetY);

/* SVR get time nano */
SVRP_EXPORT uint64_t SvrGetTimeNano ();

SVRP_EXPORT SvrResult  svrGetSensorRawData(svrSensorRawData *raw);
/* SVR : set render thread attributes */
SVRP_EXPORT void SvrSetRenderThreadAttributesByType(bool HighPriority);

/* SVR : set / get max prediction time in mm seconds */
SVRP_EXPORT void SvrSetMaxPredictTime(float maxPredictTimeMM);

SVRP_EXPORT float SvrGetMaxPredictTime();

SVRP_EXPORT void SvrCustomWarpmesh (int meshSide/* 0=left, 1=right*/,
                        unsigned int* pIndices, //pIndices : triangle[] 数组
                        int nIndices, //nIndices : nIndices 数组的元素个数
                        const void* pVertexData, //pVertexData : warpMeshVert[] 结构数组
                        int nVertices//nVertices : pVertexData 数组的元素个数
);

SVRP_EXPORT void svrResetWarpMesh ();

SVRP_EXPORT void SvrSetCustomConfig(int type);


enum SvrINTParam{
	EyeBufferWidth =0 ,
	EyeBufferHeight=1 ,
	WarpMeshType = 2 , 
	WarpMeshRows = 3 , 
	WarpMeshCols = 4
};

enum SvrBoolParam{
	EnableTimeWarp =0 ,
	DisableLensCorrection = 1 ,
	TimeWarpDelayBetweenEyes = 2
};

enum SvrFloatParam{
	LeftFrustum_Near =0 ,
	LeftFrustum_Far = 1 ,
	LeftFrustum_Left = 2,
	LeftFrustum_Right = 3, 
	LeftFrustum_Top = 4,
	LeftFrustum_Bottom = 5,
	RightFrustum_Near = 6,
	RightFrustum_Far = 7,
	RightFrustum_Left = 8,
	RightFrustum_Right = 9,
	RightFrustum_Top = 10,
	RightFrustum_Bottom = 11,
	EyeBufferFovX = 12,
	EyeBufferFovY =13
};


SVRP_EXPORT bool SvrSetBool(int type,bool value);

SVRP_EXPORT bool SvrSetInt(int type,int value);

SVRP_EXPORT bool SvrSetFloat(int type,float value);



#ifdef __cplusplus 
}
#endif

#endif //_SVR_API_H_
