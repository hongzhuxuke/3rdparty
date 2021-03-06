#include "VHGPUImageFilter.h"
#include "VHShaderUtil.h"

namespace vhall {
  std::string VHGPUImageFilter::NO_FILTER_VERTEX_SHADER = ""
    "attribute vec4 position;\n"
    "attribute vec4 inputTextureCoordinate;\n"
    " \n"
    "varying vec2 textureCoordinate;\n"
    " \n"
    "void main()\n"
    "{\n"
    "    gl_Position = position;\n"
    "    textureCoordinate = inputTextureCoordinate.xy;\n"
    "}";

  std::string VHGPUImageFilter::NO_FILTER_FRAGMENT_SHADER = ""
    "varying highp vec2 textureCoordinate;\n"
    " \n"
    "uniform sampler2D inputImageTexture;\n"
    " \n"
    "void main()\n"
    "{\n"
    "     gl_FragColor = texture2D(inputImageTexture, textureCoordinate);\n"
    "}";

  // 创建顶点 Buffer，创建纹理 Buffer
  const float VHGPUImageFilter::cubes[8] = { // 顶点坐标
            -1.0f, -1.0f, // 左下角坐标
            1.0f, -1.0f,  // 右下角坐标
            -1.0f, 1.0f,  // 左上角坐标
            1.0f, 1.0f,   // 右上角坐标
  };
  const float VHGPUImageFilter::textureCoords[8] = { // 纹理坐标
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
  };

  VHGPUImageFilter::VHGPUImageFilter() :VHGPUImageFilter(NO_FILTER_VERTEX_SHADER, NO_FILTER_FRAGMENT_SHADER) {

  }

  VHGPUImageFilter::VHGPUImageFilter(std::string vertexShader, std::string fragmentShader) {
    this->vertexShader = vertexShader;
    this->fragmentShader = fragmentShader;
    bInitialized = false;
  }

  VHGPUImageFilter::~VHGPUImageFilter() {
    destroy();
  }

  void VHGPUImageFilter::init() {
    onInit();
    onInitialized();
  }

  void VHGPUImageFilter::ifNeedInit() {
    if (!bInitialized) {
      init();
    }
  }

  void VHGPUImageFilter::destroy() {
    bInitialized = false;
    glDeleteProgram(glProgId);
    onDestroy();
  }

  void VHGPUImageFilter::onOutputSizeChanged(int width, int height) {
    outputWidth = width;
    outputHeight = height;
  }

  void VHGPUImageFilter::onDraw(int textureId, const GLfloat * cubeBuffer, const GLfloat * textureBuffer) {
    glUseProgram(glProgId); // 调用着色器程序
    checkGLError("glUseProgram");
    runPendingOnDrawTasks();
    if (!bInitialized) {
      return;
    }
    /* apply vertex */
    if (textureId != -1) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, textureId);
      glBindVertexArray(vao); // 执行一次绑定的VAO的渲染
      glUniform1i(glUniformTexture, 0);
      checkGLError("texture2");
    }
    /* draw */
    onDrawArraysPre();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    checkGLError("glDrawArrays");
    glFlush();
    glBindVertexArray(0); // vao
  }

  void VHGPUImageFilter::onInit() {
    glGenVertexArrays(1, &vao); // 创建VAOc
    glBindVertexArray(vao); // 绑定 激活

    glProgId = VHShaderUtil::createProgram(vertexShader, fragmentShader);
    glAttribPosition = glGetAttribLocation(glProgId, "position");
    checkGLError("glGetAttribLocation postion");

    glUniformTexture = glGetUniformLocation(glProgId, "inputImageTexture");
    checkGLError("glGetUniformLocation inputImageTexture");

    glAttribTextureCoordinate = glGetAttribLocation(glProgId, "inputTextureCoordinate");
    checkGLError("glGetAttribLocation inputTextureCoordinate");

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 8 * 4 * 2, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 32, cubes);
    glBufferSubData(GL_ARRAY_BUFFER, 32, 32, textureCoords);
    /* 解释顶点数据 */
    glVertexAttribPointer(glAttribPosition, 2, GL_FLOAT, false, 2 * sizeof(float), (void*)0);
    /*
     * 第一个参数为顶点着色器中layout (location=0) in vec3 position;中的location的值。
     * 第二个参数为第二个参数指定顶点属性的维数，如果是vec3，它由3个值组成，所以大小是3。
     * 第三个参数为数据的类型。
     * 第四个参数为是否希望数据被标准化，如果我们设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间。
     * 第五个参数叫做步长(Stride)，它告诉我们在连续的顶点属性组之间的间隔。设置为0的意思是让OpenGL自己去识别步长。
     * 最后一个参数表示位置数据在缓冲中起始位置的偏移量(Offset)。由于位置数据在数组的开头，所以这里是0
    */
    checkGLError("glVertexAttribPointer postion");
    glEnableVertexAttribArray(glAttribPosition); // 使vbo的内存变为可用状态
    checkGLError("glEnableVertexAttribArray postion");
    glVertexAttribPointer(glAttribTextureCoordinate, 2, GL_FLOAT, false, 2 * sizeof(float), (void*)(sizeof(cubes)));
    checkGLError("glVertexAttribPointer Coordinate");
    glEnableVertexAttribArray(glAttribTextureCoordinate);
    checkGLError("glEnableVertexAttribArray Coordinate");

    glBindBuffer(GL_ARRAY_BUFFER, 0); // vbo
    glBindVertexArray(0); // vao

    bInitialized = true;
  }

  void VHGPUImageFilter::onInitialized() {
    if (!bInitialized) init();
  }

  void VHGPUImageFilter::onDestroy() {}

  void VHGPUImageFilter::onDrawArraysPre() {}

  void VHGPUImageFilter::runPendingOnDrawTasks() {
    std::unique_lock<std::mutex> l(runOnDrawLock);
    while (!runOnDraw.empty()) {
      auto it = runOnDraw.begin();
      IRunnable* r = *it;
      runOnDraw.pop_front();
      r->run();
      delete r;
    }
  }

  void VHGPUImageFilter::setInteger(int location, int intValue) {
    IRunnable* r = new VHRunnable1i(location, intValue);
    std::lock_guard<std::mutex> l(runOnDrawLock);
    runOnDraw.push_back(r);
  }

  void VHGPUImageFilter::setFloat(int location, float floatValue) {
    IRunnable* r = new VHRunnable1f(location, floatValue);
    std::lock_guard<std::mutex> l(runOnDrawLock);
    runOnDraw.push_back(r);
  }

  void VHGPUImageFilter::setFloatVec2(int location, float * arrayValue) {
    IRunnable* r = new VHRunnable2fv(location, arrayValue);
    std::lock_guard<std::mutex> l(runOnDrawLock);
    runOnDraw.push_back(r);
  }

  void VHGPUImageFilter::setFloatVec3(int location, float * arrayValue) {
    IRunnable* r = new VHRunnable3fv(location, arrayValue);
    std::lock_guard<std::mutex> l(runOnDrawLock);
    runOnDraw.push_back(r);
  }

  void VHGPUImageFilter::setFloatVec4(int location, float * arrayValue) {
    IRunnable* r = new VHRunnable4fv(location, arrayValue);
    std::lock_guard<std::mutex> l(runOnDrawLock);
    runOnDraw.push_back(r);
  }

  void VHGPUImageFilter::setFloatArray(int location, float * arrayValue, int len) {
    IRunnable* r = new VHRunnableXfv(location, arrayValue, len);
    std::lock_guard<std::mutex> l(runOnDrawLock);
    runOnDraw.push_back(r);
  }

  void VHGPUImageFilter::setPoint(int location, float x, float y) {
    IRunnable* r = new VHRunnablePt(location, x, y);
    std::lock_guard<std::mutex> l(runOnDrawLock);
    runOnDraw.push_back(r);
  }

  void VHGPUImageFilter::setUniformMatrix3f(int location, float * matrix) {
    IRunnable* r = new VHRunnableMatrix3f(location, matrix);
    std::lock_guard<std::mutex> l(runOnDrawLock);
    runOnDraw.push_back(r);
  }

  void VHGPUImageFilter::setUniformMatrix4f(int location, float * matrix) {
    IRunnable* r = new VHRunnableMatrix4f(location, matrix);
    std::lock_guard<std::mutex> l(runOnDrawLock);
    runOnDraw.push_back(r);
  }

  void VHGPUImageFilter::appendRunOnDraw(vhall::IRunnable * r) {
    std::lock_guard<std::mutex> l(runOnDrawLock);
    runOnDraw.push_back(r);
  }

  void VHGPUImageFilter::checkGLError(const std::string & op) {
    VHShaderUtil::checkGlError(op);
  }
}
