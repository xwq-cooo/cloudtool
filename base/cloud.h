/**
 * @file cloud.h
 * @author hjm (hjmalex@163.com)
 * @version 3.0
 * @date 2022-05-08
 */
#ifndef CT_BASE_CLOUD_H
#define CT_BASE_CLOUD_H

#include "base/exports.h"

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/console/time.h>

#include <QFileInfo>
#include <QString>

#define CLOUD_TYPE_XYZ          "XYZ"
#define CLOUD_TYPE_XYZRGB       "XYZRGB"
#define CLOUD_TYPE_XYZN         "XYZNormal"
#define CLOUD_TYPE_XYZRGBN      "XYZRGBNormal"

#define BOX_PRE_FLAG                "-box"
#define NORMALS_PRE_FLAG            "-normals"

namespace ct
{
    typedef pcl::Indices Indices;
    typedef pcl::console::TicToc TicToc;
    typedef pcl::PointXYZRGBNormal PointXYZRGBN;

    struct Box
    {
        double width;
        double height;
        double depth;
        Eigen::Affine3f pose;
        Eigen::Vector3f translation;
        Eigen::Quaternionf rotation;
    };

    struct RGB
    {
        RGB() {}
        RGB(int r_, int g_, int b_) :r(r_), g(g_), b(b_) {}
        double rf() const { return (double)r / 255; }
        double gf() const { return (double)g / 255; }
        double bf() const { return (double)b / 255; }
        int r;
        int g;
        int b;
    };

    // define color
    namespace Color
    {
        const RGB White = { 255,255,255 };
        const RGB Black = { 0  ,  0,  0 };
        const RGB Red = { 255,0,  0 };
        const RGB Green = { 0,  255,0 };
        const RGB Blue = { 0,  0,  255 };
        const RGB Yellow = { 255,255,0 };
        const RGB Cyan = { 0,255,255 };
        const RGB Purple = { 255,0,255 };
    }


    class CT_EXPORT Cloud : public pcl::PointCloud<PointXYZRGBN>
    {
    public:
        Cloud() : m_id("cloud"),
            m_box_rgb(Color::White),
            m_normals_rgb(Color::White),
            m_type(CLOUD_TYPE_XYZ),
            m_point_size(1),
            m_opacity(1.0),
            m_resolution(0.0)
        {}

        Cloud(const Cloud& cloud, const Indices& indices)
            : pcl::PointCloud<PointXYZRGBN>(cloud, indices) {}

        Cloud& operator+=(const Cloud& rhs)
        {
            concatenate((*this), rhs);
            return (*this);
        }

        Cloud operator+(const Cloud& rhs) { return (Cloud(*this) += rhs); }

        using Ptr = std::shared_ptr<Cloud>;
        using ConstPtr = std::shared_ptr<const Cloud>;

        Ptr makeShared() const { return Ptr(new Cloud(*this)); }

        /**
         * @brief 点云包围盒
         */
        Box box() const { return m_box; }

        /**
         * @brief 点云ID
         */
        QString id() const { return m_id; }

        /**
         * @brief 点云法线ID
         */
        QString normalId() const { return m_id + NORMALS_PRE_FLAG; }

        /**
         * @brief 点云包围盒ID
         */
        QString boxId() const { return m_id + BOX_PRE_FLAG; }

        /**
         * @brief 点云包围盒颜色
         */
        RGB boxColor() const { return m_box_rgb; }

        /**
         * @brief 点云法线颜色
         */
        RGB normalColor() const { return m_normals_rgb; }

        /**
         * @brief 点云中心
         */
        Eigen::Vector3f center() const { return m_box.translation; }

        /**
         * @brief 点云类型
         */
        QString type() const { return m_type; }

        /**
         * @brief 点云文件信息
         */
        QFileInfo info() const { return m_info; }

        /**
         * @brief 点云文件路径
         */
        QString path() const { return m_info.path(); }

        /**
         * @brief 点云最小点
         */
        PointXYZRGBN min() const { return m_min; }

        /**
         * @brief 点云最大点
         */
        PointXYZRGBN max() const { return m_max; }

        /**
         * @brief 点云文件大小（KB)
         */
        int fileSize() const { return m_info.size() / 1024; }

        /**
         * @brief 点云点大小
         */
        int pointSize() const { return m_point_size; }

        /**
         * @brief 点云透明度
         */
        float opacity() const { return m_opacity; }

        /**
         * @brief 点云分辨率
         */
        float resolution() const { return m_resolution; }

        /**
         * @brief 点云体积
         */
        float volume() const { return m_box.depth * m_box.height * m_box.width; }

        /**
         * @brief 点云是否有法线
         */
        bool hasNormals() const { return points[rand() % size()].normal_x != 0.0f; }

        /**
         * @brief 设置点云ID
         */
        void setId(const QString& id) { m_id = id; }

        /**
         * @brief 设置点云包围盒
         */
        void setBox(const Box& box) { m_box = box; }

        /**
         * @brief 设置点云文件信息
         */
        void setInfo(const QFileInfo& info) { m_info = info; }

        /**
         * @brief 设置点云点大小
         */
        void setPointSize(int point_size) { m_point_size = point_size; }

        /**
         * @brief 设置点云点颜色 (rgb)
         */
        void setCloudColor(const RGB& rgb);

        /**
         * @brief 设置点云点颜色 (aixs)
         */
        void setCloudColor(const QString& aixs);

        /**
         * @brief 设置包围盒颜色
         */
        void setBoxColor(const RGB& rgb) { m_box_rgb = rgb; }

        /**
         * @brief 设置法线颜色
         */
        void setNormalColor(const RGB& rgb) { m_normals_rgb = rgb; }

        /**
         * @brief 设置点云透明度
         */
        void setOpacity(float opacity) { m_opacity = opacity; }

        /**
         * @brief 按照维度(x,y,z)缩放点云尺寸
         * @param origin  是否以坐标原点为缩放中心
         */
        void scale(double x, double y, double z, bool origin = false);

        /**
         * @brief 更新点云
         * @param resolution_flag 是否更新分辨率
         * @param box_flag 是否更新包围盒
         * @param type_flag 是否更新点云类型
         */
        void update(bool resolution_flag = true, bool box_flag = true, bool type_flag = true);

    private:
        Box m_box;
        QString m_id;
        RGB m_box_rgb;
        RGB m_normals_rgb;
        QString m_type;
        QFileInfo m_info;
        int m_point_size;
        float m_opacity;
        float m_resolution;
        PointXYZRGBN m_min;
        PointXYZRGBN m_max;
    };
} // namespace ct
#endif // CT_BASE_CLOUD_H