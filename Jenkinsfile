pipeline {
    agent {
        label 'vps'
    }
    stages {
    /**      
        stage('Send starting notification') {
            steps {
                sh '''# Notify about the new release
                    curl -H "Authorization: Basic bXJ1d3p1bTo4XkxBRm1maGNBUzQ5MlBFNFU0elBRIThxMypKJWUjZl4=" \
                         -H "Title: XtremeFW release available" \
                         -H "Priority: max" \
                         -H "Tags: dolphin" \
                         -H "Actions: view, Open Jenkins console, $BUILD_URL/console" \
                         -d "A new firmware is ready for your Flipper Zero. Building custom FW..." \
                         "https://miguelangelantolin.dynamic-dns.net/flipper"'''
            }
        } 
    */
        stage('Clone repository') {
            steps {
                sh "git clone --recursive --jobs 8 -b main https://github.com/mruwzum/Xtreme-Firmware.git"
            }
        }
        stage('Checkout & merge changes') {
            steps {
                sh '''cd Xtreme-Firmware
                    git remote add upstream https://github.com/Flipper-XFW/Xtreme-Firmware.git
                    git fetch upstream
                    git merge upstream/main'''
            }
        }
        stage('Compile FW') {
            steps {
                sh '''cd Xtreme-Firmware
                    ./fbt updater_package'''
            }
        }
        stage('Send complete notification') {
            steps {
                sh '''curl -H "Authorization: Basic bXJ1d3p1bTo4XkxBRm1maGNBUzQ5MlBFNFU0elBRIThxMypKJWUjZl4=" \
                     -H "Title: Build complete" \
                     -H "Priority: max" \
                     -H "Tags: building_construction" \
                     -H "Actions: view, Open in Jenkins, $BUILD_URL" \
                     -d "Custom XtremeFW build released." \
                     "https://miguelangelantolin.dynamic-dns.net/jenkins"'''
            }
        }
    }
    post {
        always {
            script {
                step($class: 'ArtifactArchiver', followSymlinks: false, artifacts: 'Xtreme-Firmware/dist/f7-C/*.tgz')
                // Remove the workspace after execution
                deleteDir()
            }
        }
    }
}
