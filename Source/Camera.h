

#ifndef CAMERA_H
#define CAMERA_H

#include "ExMatrix3D.h"

static float radiansToDegrees(float rads) noexcept { return (180.0f / juce::MathConstants<float>::pi) * rads; }
static float degreesToRadians(float degs) noexcept { return (juce::MathConstants<float>::pi / 180.0f) * degs; }

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 1.0f;
const float ZOOM = 45.0f;

using namespace juce;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    Vector3D<float> Position;
    Vector3D<float> Front;
    Vector3D<float> Up;
    Vector3D<float> Right;
    Vector3D<float> WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    float lastX = 0.0f, lastY = 0.0f;

    // constructor with vectors
    Camera(Vector3D<float> position = Vector3D<float>(0.0f, 0.0f, 0.0f), Vector3D<float> up = Vector3D<float>(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(Vector3D<float>(0.0f, 0.0f, -3.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(Vector3D<float>(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = Vector3D(posX, posY, posZ);
        WorldUp = Vector3D(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    Matrix3D<float> GetViewMatrix()
    {
        return ExMatrix3D<float>::fromLookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

    void updateMousePosition(const juce::Point<int>& position)
    {
        // Handle the mouse position
        mousePosition = position;

        //setMouseCursor(MouseCursor::NoCursor);
 
        if (firstMouse)
        {
            lastX = mousePosition.x;
            lastY = mousePosition.y;
            firstMouse = false;
        }

        float xoffset = mousePosition.x - lastX;
        float yoffset = lastY - mousePosition.y;
        lastX = mousePosition.x;
        lastY = mousePosition.y;

        ProcessMouseMovement(xoffset, yoffset);
    }

    juce::Point<int> getMousePosition() const
    {
        return mousePosition;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        Vector3D<float> front;
        front.x = cos(degreesToRadians(Yaw)) * cos(degreesToRadians(Pitch));
        front.y = sin(degreesToRadians(Pitch));
        front.z = sin(degreesToRadians(Yaw)) * cos(degreesToRadians(Pitch));
        Front = front.normalised();
        // also re-calculate the Right and Up vector
        Right = (Front ^ WorldUp).normalised();  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = (Right ^ Front).normalised();
    }

    juce::Point<int> mousePosition;

    bool firstMouse = true;
};
#endif

