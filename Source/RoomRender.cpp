/*
  ==============================================================================

    RoomRender.cpp
    Created: 21 Jul 2024 11:24:14am
    Author:  jstan

  ==============================================================================
*/

#include <JuceHeader.h>
#include "RoomRender.h"
#include "ExMatrix3D.h"

//==============================================================================
RoomRender::RoomRender()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    setSize(width, height);
    addKeyListener(this);
    setWantsKeyboardFocus(true);

    myMouseListener = std::make_unique<MyMouseListener>();
    myMouseListener->setMyClassInstance(&camera);
    addMouseListener(myMouseListener.get(), true);
}

RoomRender::~RoomRender()
{
    shutdownOpenGL();
}

void RoomRender::initialise()
{
    // Initialize OpenGL resources here
    createShaders();

    //std::vector<float> walls{
    //    //Position            //Texture                //ID
    //    -0.5f, -0.5f, -0.5f,  0.0f,       0.0f,        0.0f,
    //     0.5f, -0.5f, -0.5f,  roomSize.x, 0.0f,        0.0f,
    //     0.5f,  0.5f, -0.5f,  roomSize.x, roomSize.y,  0.0f,
    //    -0.5f,  0.5f, -0.5f,  0.0f,       roomSize.y,  0.0f,

    //    -0.5f, -0.5f,  0.5f,  0.0f,       0.0f,        0.0f,
    //     0.5f, -0.5f,  0.5f,  roomSize.x, 0.0f,        0.0f,
    //     0.5f,  0.5f,  0.5f,  roomSize.x, roomSize.y,  0.0f,
    //    -0.5f,  0.5f,  0.5f,  0.0f,       roomSize.y,  0.0f,

    //    -0.5f,  0.5f,  0.5f,  0.0f,       0.0f,        0.0f,
    //    -0.5f,  0.5f, -0.5f,  roomSize.z, 0.0f,        0.0f,
    //    -0.5f, -0.5f, -0.5f,  roomSize.z, roomSize.y,  0.0f,
    //    -0.5f, -0.5f,  0.5f,  0.0f,       roomSize.y,  0.0f,

    //     0.5f,  0.5f,  0.5f,  0.0f,       0.0f,        0.0f,
    //     0.5f,  0.5f, -0.5f,  roomSize.z, 0.0f,        0.0f,
    //     0.5f, -0.5f, -0.5f,  roomSize.z, roomSize.y,  0.0f,
    //     0.5f, -0.5f,  0.5f,  0.0f,       roomSize.y,  0.0f,
    //};

    //std::vector<float> floor{
    //    //Position            //Texture                //ID
    //    -0.5f, -0.5f, -0.5f,  0.0f,       roomSize.z,  1.0f,
    //     0.5f, -0.5f, -0.5f,  roomSize.x, roomSize.z,  1.0f,
    //     0.5f, -0.5f,  0.5f,  roomSize.x, 0.0f,        1.0f,
    //    -0.5f, -0.5f,  0.5f,  0.0f,       0.0f,        1.0f,
    //};

    //std::vector<float> ceiling{
    //    //Position            //Texture                //ID
    //    -0.5f,  0.5f, -0.5f,  0.0f,       roomSize.z,  2.0f,
    //     0.5f,  0.5f, -0.5f,  roomSize.x, roomSize.z,  2.0f,
    //     0.5f,  0.5f,  0.5f,  roomSize.x, 0.0f,        2.0f,
    //    -0.5f,  0.5f,  0.5f,  0.0f,       0.0f,        2.0f,
    //};

    auto& sharedData = SharedDataSingleton::getInstance();
    std::lock_guard<std::mutex> lock(sharedData.vectorMutex);
    roomSize = juce::Vector3D<float>(30.0f, 30.0f, 30.0f);
    sharedData.roomSize = roomSize;
    sharedData.soundSourcePos = juce::Vector3D<float>(18.0f, 18.0f, 18.0f);

    //shape->roomSize = roomSize;
    cameraPos = Vector3D<float>(2.0f, 2.0f, 2.0f);
    sharedData.listenerPos = cameraPos;

    //Make the listener box scale with the room to help manage the processing overhead
    sharedData.listenerSize = roomSize / 20.0f;
    roomPos = roomSize / 2;
    sharedData.roomPos = roomPos;

    camera = Camera(cameraPos, Vector3D<float>(0.0f, 1.0f, 0.0f), 0.0f);

    camera.lastX = width / 2;
    camera.lastY = height / 2;

    //Add shapes
    shape->addShapes(sharedData.walls, sharedData.floor, sharedData.ceiling, roomSize);
}

void RoomRender::shutdown()
{
    // Clean up OpenGL resources here
    shader.reset();
    shape->texture1.release();
    shape->texture2.release();
    shape->texture3.release();
    shape.reset();
    attributes.reset();
    uniforms.reset();
}

void RoomRender::render()
{
    // Your OpenGL rendering code here
    // This clears the context with a black background.
    juce::OpenGLHelpers::clear(juce::Colours::black);

    // Add your rendering code here...
    using namespace ::juce::gl;

    jassert(juce::OpenGLHelpers::isContextActive());

    auto desktopScale = (float)openGLContext.getRenderingScale();
    juce::OpenGLHelpers::clear(juce::OpenGLAppComponent::getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    if (shape->textures[0] != nullptr)
        shape->textures[0]->applyTo(shape->texture1);
    if (shape->textures[1] != nullptr)
        shape->textures[1]->applyTo(shape->texture2);
    if (shape->textures[2] != nullptr)
        shape->textures[2]->applyTo(shape->texture3);

    glEnable(GL_DEPTH_TEST);

    width = (float)juce::OpenGLAppComponent::getWidth();
    height = (float)juce::OpenGLAppComponent::getHeight();

    glViewport(0, 0,
        juce::roundToInt(desktopScale * width),
        juce::roundToInt(desktopScale * height));

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    shader->use();

    Matrix3D<float> projection;
    projection = ExMatrix3D<float>::fromPerspective(degreesToRadians(camera.Zoom), (width / height), 0.1f, 100.0f);
    if (uniforms->projection.get() != nullptr)
        uniforms->projection->setMatrix4(projection.mat, 1, false);

    Matrix3D<float> view;
    view = ExMatrix3D<float>::fromLookAt(camera.Position, camera.Position + camera.Front, camera.Up);
    if (uniforms->view.get() != nullptr)
        uniforms->view->setMatrix4(view.mat, 1, false);

    ExMatrix3D<float> model;
    model = model.translation(roomPos);
    model = model.scaled(roomSize);
    if (uniforms->model.get() != nullptr)
        uniforms->model->setMatrix4(model.mat, 1, false);

    if (uniforms->texture1 != nullptr)
        uniforms->texture1->set((GLint)0);
    if (uniforms->texture2 != nullptr)
        uniforms->texture2->set((GLint)1);
    if (uniforms->texture3 != nullptr)
        uniforms->texture3->set((GLint)2);

    shape->draw(*attributes);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void RoomRender::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

}

void RoomRender::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void RoomRender::createShaders()
{
    vertexShader = R"(
        attribute vec3 position;
        attribute vec2 texCoord;
        attribute float type;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        varying vec2 texCoordOut;
        varying float theType;

        void main()
        {
            //Note that we read the multiplication from right to left
            gl_Position = projection * view * model * vec4(position, 1.0f);
            texCoordOut = vec2(texCoord.x, texCoord.y);
            theType = type;
        }
        )";

    fragmentShader = R"(
        varying vec2 texCoordOut;
        varying float theType;

        uniform sampler2D texture1;
        uniform sampler2D texture2;
        uniform sampler2D texture3;

        int theTypeI = int(theType);
  
        void main()
        {
            if (theTypeI == 0)
                gl_FragColor = texture(texture1, texCoordOut);
            if (theTypeI == 1)
                gl_FragColor = texture(texture2, texCoordOut);
            if (theTypeI == 2)
                gl_FragColor = texture(texture3, texCoordOut);
            //gl_FragColor = vec4(0.95, 0.57, 0.03, 0.7);
        }
        )";

    std::unique_ptr<juce::OpenGLShaderProgram> newShader(new juce::OpenGLShaderProgram(openGLContext));
    juce::String statusText;

    if (newShader->addVertexShader(juce::OpenGLHelpers::translateVertexShaderToV3(vertexShader))
        && newShader->addFragmentShader(juce::OpenGLHelpers::translateFragmentShaderToV3(fragmentShader))
        && newShader->link())
    {
        shape.reset();
        attributes.reset();
        uniforms.reset();

        shader.reset(newShader.release());
        shader->use();

        shape.reset(new Shape());
        attributes.reset(new Attributes(*shader));
        uniforms.reset(new Uniforms(*shader));

        statusText = "GLSL: v" + juce::String(juce::OpenGLShaderProgram::getLanguageVersion(), 2);
    }
    else
    {
        statusText = newShader->getLastError();
    }
}

bool RoomRender::keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent)
{
    if (key == juce::KeyPress::escapeKey)
    {
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
        return true;
    }
    return false;
}
