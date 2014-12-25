#include "File.h"
#include <vector>

using namespace std;
namespace octet
{

  float colorfactor=1.0f;

  class Tree
  {
    public:
    mat4t modelToWorld;

    float length;
    vec4 color;
  
    void init(vec4 &col, float h, mat4t &m )
    {
      color = col;
      length = h;
      modelToWorld=m;
    }

    void render(mat4t &cameraToWorld, my_shader &c_shader)
    {
          
        mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);
    
        // set up the uniforms for the shader
        c_shader.render(modelToProjection, color, colorfactor);
    
        float vertices[] = {
          0,  0,   
          0,  length,   
          
        };

        glVertexAttribPointer(attribute_pos, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)vertices );
        glEnableVertexAttribArray(attribute_pos);

        glDrawArrays(GL_LINES, 0, 2);
      }
  
  };

  
  class Rule
  {
      public:
        char begin[1];
        std::string equalTo;
  };
  
  class LSystem : public octet::app
  {
     public:
     File file;
     Rule rule, node;
     Tree tree;

     public:
     std::string axiom;
     std::string astr;
     int maxIteration, currentIteration;
     int cam_x, cam_y, cam_z;
     float length, angle;
     mat4t cameraToWorld;
     mat4t modelToWorld;
     my_shader col_shader;
     vec4 color;  
   
     dynarray <mat4t> thePosition;
     vector <Rule> ruleList;
     dynarray <Tree> lines;

     LSystem(int argc, char **argv) : app(argc, argv){
     }

     void init_axiom()
     {
        tree.init(vec4(0.0f, 0.5f, 0.0f, 0.8f), length, modelToWorld);
     }

     void init_node()
     {
        tree.init(vec4(0.0f, 0.5f, 0.0f, 0.8f), length, modelToWorld);
     }

     void reset () {
        thePosition.reset();
        lines.reset();
        astr = axiom;  
     }

     void manageStrings()
     {
      axiom = file.getAxiom();
      astr = axiom; 
      char *c_axiom;
      c_axiom = &axiom[0];
      std::string axiom2 = file.getAxiom2();
      char *c_axiom2;
      c_axiom2 = &axiom2[0];
      rule.begin[0] = *c_axiom;
      rule.equalTo = file.getRule1();
      node.begin[0] = *c_axiom2;
      node.equalTo = file.getRule2();
      ruleList.push_back(rule);
      ruleList.push_back(node);
     }

     void setParameters()
     {
       manageStrings();
       maxIteration = file.getIteration();
       currentIteration = file.getIteration();
       angle = file.getAngle();
     }
           
     void drawForward()
     {
        lines.push_back(tree);
        modelToWorld.translate(0,length,0);
     }

     void rotateRight()
     {
        modelToWorld.rotateZ(-angle);     
     }

     void rotateLeft()
     {
        modelToWorld.rotateZ(angle);     
     }

     void push()
     {
        thePosition.push_back(modelToWorld); 
     }

     void pop()
     {
        modelToWorld = thePosition.back();
        thePosition.pop_back();
     }

     void implementCamera()
     {
        cameraToWorld.loadIdentity();
        cameraToWorld.translate(0, 9, 10);
     }
       
     void checkRule()
     {
       for (int it = 0; it < maxIteration; ++it){
         for (int i = astr.length()-1; i >= 0; --i){
           for (int rl = 0; rl < ruleList.size(); ++rl){
             if (astr[i] == *ruleList[rl].begin){
               astr.replace(i, 1, ruleList[rl].equalTo);
             }
           }
         }
       }

       for(int i=0; i < astr.length(); i++)
       {
         if(astr[i] == 'F') //draw forward
         {
          init_axiom();
          drawForward();
         } 
         else if(astr[i] == 'X') //draw forward
         {
          init_node();
          drawForward();
          
         }
         else if(astr[i] == '+') //rotate left
         {
          rotateLeft();
         }
         else if(astr[i] == '-') //rotate right
         {
          rotateRight();
         }
         else if(astr[i] == '[') //push
         {
          push();
         }
         else if(astr[i] == ']') //pop
         {   
          pop();
         }
       }
    }
     void app_init()
     {
        col_shader.init();
        implementCamera();
        length = 0.07f;
        modelToWorld.loadIdentity();
        setParameters();
        checkRule();
     }

     void simulate(){
       if(is_key_down(key_up)) //increase iteration
       {
         if(currentIteration > maxIteration )
         { 
          maxIteration++;
          cout<<"Max iteration "<< maxIteration <<endl;
          reset();
          modelToWorld.loadIdentity();
          checkRule();   
          Sleep(100);
         }
       }
       else if(is_key_down(key_left)) //decrease branch angle
       {
          reset();
          modelToWorld.loadIdentity(); 
          angle -= 1.0f;
          checkRule();
          Sleep(0);
       }
       else if(is_key_down(key_right)) //increase branch angle
       {
          reset();
          modelToWorld.loadIdentity();
          angle += 1.0f;
          checkRule();
          Sleep(0);
       }
       else if(is_key_down(key_down)) //decrease iteration
       {
        if(maxIteration > 0)
        {
          reset();
          maxIteration--;
          modelToWorld.loadIdentity();
          checkRule();
          Sleep(0);
        }
       }
       else if(is_key_down('W')) //move up
       {
          cameraToWorld.translate(0, -0.1f, 0);
       }
       else if(is_key_down('S'))//move down
       {
          cameraToWorld.translate(0, +0.1f, 0);
       }
       else if(is_key_down('A'))//move left
       {
          cameraToWorld.translate(+0.1f, 0, 0);
       }
       else if(is_key_down('D'))//move right
       {
          cameraToWorld.translate(-0.1f, 0, 0);
       }
       else if(is_key_down('Z'))//zoom out
       {
          cameraToWorld.translate(0, 0, +0.1f);
          cout << cam_z <<endl;
       }
       else if(is_key_down('X'))//zoom in
       {
          cameraToWorld.translate(0, 0, -0.1f);
          cout << cam_z <<endl;
       }
       else if(is_key_down('Q')) //change length
       {  
          reset();
          length -= 0.001f;
          modelToWorld.loadIdentity();
          checkRule();
       }
       else if(is_key_down('E')) //change length
       {  
          reset();
          length += 0.001f;
          modelToWorld.loadIdentity();
          checkRule();
       }
       else if(is_key_down(key_shift)) //increase contrast
       {
          colorfactor+=0.05f;
       }
       else if(is_key_down(key_ctrl)) //decrease contrast
       {
          colorfactor-=0.05f;
       }
       else if(is_key_down(key_f1)) //load first tree
       {
          reset();
          ruleList.clear();
          file.chooseFile(1);
          setParameters();
          checkRule();
       }
       else if(is_key_down(key_f2)) //load second tree
       {
          reset();
          ruleList.clear();
          file.chooseFile(2);
          setParameters();
          modelToWorld.loadIdentity();
          checkRule();
       }
       else if(is_key_down(key_f3)) //load third tree
       {
          reset();
          ruleList.clear();
          file.chooseFile(3);
          setParameters();
          modelToWorld.loadIdentity();
          checkRule();
       }
       else if(is_key_down(key_f4)) //load fourth tree
       {
          reset();
          ruleList.clear();
          file.chooseFile(4);
          setParameters();
          modelToWorld.loadIdentity();
          checkRule();
       }
       else if(is_key_down(key_f5)) //load fifth tree
       {
          reset();
          ruleList.clear();
          file.chooseFile(5);
          setParameters();
          modelToWorld.loadIdentity();
          checkRule();
       }
       else if(is_key_down(key_f6)) //load sixth tree
       {
          reset();
          ruleList.clear();
          file.chooseFile(6);
          setParameters();
          modelToWorld.loadIdentity();
          checkRule();
       }
     }

     void draw_world(int x, int y, int w, int h )
     {   
       simulate();
   
       glViewport(x, y, w, h);

       // clear the background to black
       glClearColor(0, 0, 0, 1);
       glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

       // don't allow Z buffer depth testing (closer objects are always drawn in front of far ones)
       glDisable(GL_DEPTH_TEST);

       // allow alpha blend (transparency when alpha channel is 0)
       glEnable(GL_BLEND);
       glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
       
       for (int i = 0; i < lines.size(); i++) {
          lines[i].render(cameraToWorld, col_shader);
       }
     }
  };
}