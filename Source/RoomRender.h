/*
  ==============================================================================

    RoomRender.h
    Created: 21 Jul 2024 11:24:14am
    Author:  jstan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Camera.h"
#include "SharedData.h"

class MyMouseListener : public juce::MouseListener
{
public:
    MyMouseListener() = default;

    void mouseMove(const juce::MouseEvent& event) override
    {
        event.source.enableUnboundedMouseMovement(true);
        bool test = event.source.canDoUnboundedMovement();
        bool test2 = event.source.isUnboundedMouseMovementEnabled();
        juce::Point<int> mousePos = event.getScreenPosition();
        // Pass the mouse position to your class
        cameraClassInstance->updateMousePosition(mousePos);

        //DBG(mousePos.x);
    }

    void mouseWheelMove(const juce::MouseEvent& event)
    {
        cameraClassInstance->ProcessMouseScroll(event.getPosition().y);
    }

    void setMyClassInstance(Camera* instance)
    {
        cameraClassInstance = instance;
    }

private:
    Camera* cameraClassInstance = nullptr;
};
//==============================================================================
/*
*/
class RoomRender  : public juce::OpenGLAppComponent, public juce::KeyListener
{
public:
    RoomRender();
    ~RoomRender() override;

    void initialise() override;
    void shutdown() override;
    void render() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void createShaders();
    bool keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent) override;

private:
    //==============================================================================
    // Your private member variables go here...

    struct Vertex
    {
        float position[3];
        float texCoord[2];
        float type;
    };

    struct Attributes
    {
        explicit Attributes(juce::OpenGLShaderProgram& shaderProgram)
        {
            position.reset(createAttribute(shaderProgram, "position"));
            texCoord.reset(createAttribute(shaderProgram, "texCoord"));
            type.reset(createAttribute(shaderProgram, "type"));
        }

        void enable()
        {
            using namespace ::juce::gl;

            if (position.get() != nullptr)
            {
                glVertexAttribPointer(position->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
                glEnableVertexAttribArray(position->attributeID);
            }

            if (texCoord.get() != nullptr)
            {
                glVertexAttribPointer(texCoord->attributeID, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 3));
                glEnableVertexAttribArray(texCoord->attributeID);
            }

            if (type.get() != nullptr)
            {
                glVertexAttribPointer(type->attributeID, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 5));
                glEnableVertexAttribArray(type->attributeID);
            }
        }

        void disable()
        {
            using namespace ::juce::gl;

            if (position.get() != nullptr) glDisableVertexAttribArray(position->attributeID);
            if (texCoord.get() != nullptr) glDisableVertexAttribArray(texCoord->attributeID);
            if (type.get() != nullptr) glDisableVertexAttribArray(type->attributeID);
        }

        std::unique_ptr<juce::OpenGLShaderProgram::Attribute> position, texCoord, type;

        static juce::OpenGLShaderProgram::Attribute* createAttribute(juce::OpenGLShaderProgram& shader,
            const juce::String& attributeName)
        {
            using namespace ::juce::gl;

            if (glGetAttribLocation(shader.getProgramID(), attributeName.toRawUTF8()) < 0)
                return nullptr;

            return new juce::OpenGLShaderProgram::Attribute(shader, attributeName.toRawUTF8());
        }
    };

    struct Uniforms
    {
        explicit Uniforms(juce::OpenGLShaderProgram& shaderProgram)
        {
            model.reset(createUniform(shaderProgram, "model"));
            view.reset(createUniform(shaderProgram, "view"));
            projection.reset(createUniform(shaderProgram, "projection"));
            texture1.reset(createUniform(shaderProgram, "texture1"));
            texture2.reset(createUniform(shaderProgram, "texture2"));
            texture3.reset(createUniform(shaderProgram, "texture3"));
        }

        std::unique_ptr<juce::OpenGLShaderProgram::Uniform> model, view, projection, texture1, texture2, texture3;

        static juce::OpenGLShaderProgram::Uniform* createUniform(juce::OpenGLShaderProgram& shaderProgram,
            const juce::String& uniformName)
        {
            using namespace ::juce::gl;

            if (glGetUniformLocation(shaderProgram.getProgramID(), uniformName.toRawUTF8()) < 0)
                return nullptr;

            return new juce::OpenGLShaderProgram::Uniform(shaderProgram, uniformName.toRawUTF8());
        }
    };

    struct Shape
    {
        Shape()
        {
        }
        
        void addShapes(std::vector<float> verticesWalls, std::vector<float> verticesFloor, std::vector<float> verticesCeiling)
        {
            //Add shapes to vertex buffer and remove old vertices if there are any
            if (vertexBuffers.size() != 0)
            {
                vertexBuffers.clear(true);
            }
            vertexBuffers.add(new VertexBuffer(verticesWalls, verticesWalls.size()/6));
            vertexBuffers.add(new VertexBuffer(verticesFloor, verticesFloor.size()/6));
            vertexBuffers.add(new VertexBuffer(verticesCeiling, verticesCeiling.size()/6));
            //Add texture(s) and remove old ones if there are any
            if (textures.size() != 0)
            {
                textures.clear(true);
            }
            textures.add(new TextureFromFile(File("C:/Users/jstan/JUCE/Projects/RoomExample/Assets/Bricks060_1K-JPG_Color.jpg")));
            textures.add(new TextureFromFile(File("C:/Users/jstan/JUCE/Projects/RoomExample/Assets/Wood090A_1K-JPG_Color.jpg")));
            textures.add(new TextureFromFile(File("C:/Users/jstan/JUCE/Projects/RoomExample/Assets/Tiles136A_1K-JPG_Color.jpg")));
        }

        void draw(Attributes& glAttributes)
        {
            using namespace ::juce::gl;

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            vertexBuffers[0]->bind();
            glAttributes.enable();
            glActiveTexture(GL_TEXTURE0);
            texture1.bind();
            glDrawArrays(GL_TRIANGLES, 0, vertexBuffers[0]->numberTriangles);
            glAttributes.disable();

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            vertexBuffers[1]->bind();
            glAttributes.enable();
            glActiveTexture(GL_TEXTURE1);
            texture2.bind();
            glDrawArrays(GL_TRIANGLES, 0, vertexBuffers[1]->numberTriangles);
            glAttributes.disable();

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            vertexBuffers[2]->bind();
            glAttributes.enable();
            glActiveTexture(GL_TEXTURE2);
            texture3.bind();
            glDrawArrays(GL_TRIANGLES, 0, vertexBuffers[2]->numberTriangles);
            glAttributes.disable();

        }

        struct VertexBuffer
        {
            explicit VertexBuffer(std::vector<float> verticesIn, int numTriangles)
            {
                using namespace ::juce::gl;

                numberTriangles = numTriangles;

                glGenBuffers(1, &vertexBuffer);
                glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

                juce::Array<Vertex> vertices;
                for (auto i = 0; i < verticesIn.size(); i += 6)
                {
                    vertices.add({ { verticesIn.at(i), verticesIn.at(i + 1), verticesIn.at(i + 2) },
                        { verticesIn.at(i + 3) , verticesIn.at(i + 4) },
                        { verticesIn.at(i + 5) } });
                }

                glBufferData(GL_ARRAY_BUFFER,
                    static_cast<GLsizeiptr> (static_cast<size_t> (vertices.size()) * sizeof(Vertex)),
                    vertices.getRawDataPointer(), GL_STATIC_DRAW);
            }

            ~VertexBuffer()
            {
                using namespace ::juce::gl;

                glDeleteBuffers(1, &vertexBuffer);
            }

            void bind()
            {
                using namespace ::juce::gl;

                glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
            }

            GLuint vertexBuffer;
            int numberTriangles;

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VertexBuffer)
        };

        struct TextureFromFile
        {
            TextureFromFile(const File& file)
            {
                name = file.getFileName();
                image = ImageFileFormat::loadFrom(file);
            }

            Image image;
            String name;

            bool applyTo(OpenGLTexture& texture)
            {
                texture.loadImage(image);
                return false;
            }
        };

        juce::OwnedArray<VertexBuffer> vertexBuffers;
        OwnedArray<TextureFromFile> textures;
        OpenGLTexture texture1, texture2, texture3;

        Vector3D<float> roomSize;

        //std::vector<float> verticesWalls{
        //    //Position            //Texture                //ID
        //    -0.5f, -0.5f, -0.5f,  0.0f,       0.0f,        0.0f,
        //     0.5f, -0.5f, -0.5f,  roomSize.x, 0.0f,        0.0f,
        //     0.5f,  0.5f, -0.5f,  roomSize.x, roomSize.y,  0.0f,
        //     0.5f,  0.5f, -0.5f,  roomSize.x, roomSize.y,  0.0f,
        //    -0.5f,  0.5f, -0.5f,  0.0f,       roomSize.y,  0.0f,
        //    -0.5f, -0.5f, -0.5f,  0.0f,       0.0f,        0.0f,

        //    -0.5f, -0.5f,  0.5f,  0.0f,       0.0f,        0.0f,
        //     0.5f, -0.5f,  0.5f,  roomSize.x, 0.0f,        0.0f,
        //     0.5f,  0.5f,  0.5f,  roomSize.x, roomSize.y,  0.0f,
        //     0.5f,  0.5f,  0.5f,  roomSize.x, roomSize.y,  0.0f,
        //    -0.5f,  0.5f,  0.5f,  0.0f,       roomSize.y,  0.0f,
        //    -0.5f, -0.5f,  0.5f,  0.0f,       0.0f,        0.0f,

        //    -0.5f,  0.5f,  0.5f,  0.0f,       0.0f,        0.0f,
        //    -0.5f,  0.5f, -0.5f,  roomSize.z, 0.0f,        0.0f,
        //    -0.5f, -0.5f, -0.5f,  roomSize.z, roomSize.y,  0.0f,
        //    -0.5f, -0.5f, -0.5f,  roomSize.z, roomSize.y,  0.0f,
        //    -0.5f, -0.5f,  0.5f,  0.0f,       roomSize.y,  0.0f,
        //    -0.5f,  0.5f,  0.5f,  0.0f,       0.0f,        0.0f,

        //     0.5f,  0.5f,  0.5f,  0.0f,       0.0f,        0.0f,
        //     0.5f,  0.5f, -0.5f,  roomSize.z, 0.0f,        0.0f,
        //     0.5f, -0.5f, -0.5f,  roomSize.z, roomSize.y,  0.0f,
        //     0.5f, -0.5f, -0.5f,  roomSize.z, roomSize.y,  0.0f,
        //     0.5f, -0.5f,  0.5f,  0.0f,       roomSize.y,  0.0f,
        //     0.5f,  0.5f,  0.5f,  0.0f,       0.0f,  0.0f
        //};

        //std::vector<float> verticesWalls{
        //    // positions         // colors
        //     0.5f, -0.5f, 0.0f,  0.0f, 0.0f,  0.0f,   // bottom right
        //    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,  0.0f,   // bottom left
        //     0.0f,  0.5f, 0.0f,  0.0f, 0.0f,  0.0f    // top 
        //};

        //std::vector<float> verticesFloor{
        //    //Position            //Texture                //ID
        //    -0.5f, -0.5f, -0.5f,  0.0f,       roomSize.z,  1.0f,
        //     0.5f, -0.5f, -0.5f,  roomSize.x, roomSize.z,  1.0f,
        //     0.5f, -0.5f,  0.5f,  roomSize.x, 0.0f,        1.0f,
        //     0.5f, -0.5f,  0.5f,  roomSize.x, 0.0f,        1.0f,
        //    -0.5f, -0.5f,  0.5f,  0.0f,       0.0f,        1.0f,
        //    -0.5f, -0.5f, -0.5f,  0.0f,       roomSize.z,  1.0f
        //};

        //std::vector<float> verticesCeiling{
        //    //Position            //Texture                //ID
        //    -0.5f,  0.5f, -0.5f,  0.0f,       roomSize.z,  2.0f,
        //     0.5f,  0.5f, -0.5f,  roomSize.x, roomSize.z,  2.0f,
        //     0.5f,  0.5f,  0.5f,  roomSize.x, 0.0f,        2.0f,
        //     0.5f,  0.5f,  0.5f,  roomSize.x, 0.0f,        2.0f,
        //    -0.5f,  0.5f,  0.5f,  0.0f,       0.0f,        2.0f,
        //    -0.5f,  0.5f, -0.5f,  0.0f,       roomSize.z,  2.0f
        //};

    };

    float width = 300.0f, height = 300.0f;

    juce::String vertexShader;
    juce::String fragmentShader;

    std::unique_ptr<juce::OpenGLShaderProgram> shader;
    std::unique_ptr<Shape> shape;
    std::unique_ptr<Attributes> attributes;
    std::unique_ptr<Uniforms> uniforms;

    Camera camera;
    Vector3D<float> cameraPos, roomPos;
    float lastX = 0.0f, lastY = 0.0f;
    bool firstMouse = true;

    juce::OpenGLContext openGLContext;

    juce::Point<int> mousePosition;
    std::unique_ptr<MyMouseListener> myMouseListener;

    std::shared_ptr<SharedData> sharedData;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RoomRender)
};
